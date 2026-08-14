#include "StageEditorUIHelper.h"
#include <numbers>
#include "StageEditor/StageSpawner/StageSpawner.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"
#include "Scene/GameScene/GameScene.h"
#include "StageObject/StaticEventTrigger/StaticEventTrigger.h"

// プレハブデータの保存先フォルダ
constexpr const char* PREFAB_DIR = "./Assets/Parameter/EditorPrefab/";

namespace StageEditorUIHelper
{
	/// @brief Prefabの名前を取得する
	/// @return 
	std::vector<std::string> GetPrefabNames()
	{
		std::vector<std::string> names;
		if (!std::filesystem::exists(PREFAB_DIR))
		{
			std::filesystem::create_directories(PREFAB_DIR);
			return names;
		}

		for (const auto& entry : std::filesystem::directory_iterator(PREFAB_DIR))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".json")
			{
				names.push_back(entry.path().stem().string());
			}
		}
		return names;
	}

	/// @brief Prefabを保存する
	/// @param name 
	/// @param data 
	void SavePrefab(const std::string& name, const PlacementData& data)
	{
		std::filesystem::create_directories(PREFAB_DIR);
		std::string filePath = std::string(PREFAB_DIR) + name + ".json";

		std::ofstream file(filePath);
		if (file.is_open())
		{
			json j;
			toJson(j, data);
			file << j.dump(4);
			file.close();
		}
	}

	/// @brief Prefabをロードする
	/// @param name 
	/// @param data 
	void LoadPrefab(const std::string& name, PlacementData& data)
	{
		std::string filePath = std::string(PREFAB_DIR) + name + ".json";
		std::ifstream file(filePath);
		if (file.is_open())
		{
			json j;
			file >> j;

			// 既存の実体ポインタなどを退避
			void* backupPtr = data.instancePtr;

			fromJson(j, data);
			data.instancePtr = backupPtr; // ポインタを復元

			file.close();
		}
	}

	/// @brief テーブルレイアウトの開始
	/// @param str_id 
	/// @return 
	bool BeginPropertyTable(const char* str_id)
	{
		// 2列のテーブルを作成（1列目はラベル用に幅固定、2列目は自動ストレッチ）
		if (ImGui::BeginTable(str_id, 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed, 130.0f);
			ImGui::TableSetupColumn("Property", ImGuiTableColumnFlags_WidthStretch);
			return true;
		}
		return false;
	}

	/// @brief テーブルレイアウトの終了
	void EndPropertyTable()
	{
		ImGui::EndTable();
	}

	/// @brief プロパティラベルを描画する
	/// @param label 
	void PropertyLabel(const char* label)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::AlignTextToFramePadding();
		ImGui::Text("%s", label);
		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(-FLT_MIN); // 入力欄を右端まで広げる
	}

	/// @brief ビヘイビアツリーのセレクターを描画する
	/// @param label 
	/// @param currentBT 
	/// @param behaviorTreeNames 
	/// @return 
	bool DrawBehaviorTreeSelector(const char* label, std::string& currentBT, const std::vector<std::string>& behaviorTreeNames)
	{
		bool isChanged = false;
		const char* previewValue = currentBT.empty() ? "選択なし" : currentBT.c_str();

		if (ImGui::BeginCombo(label, previewValue))
		{
			// 選択なしのオプションを追加
			if (ImGui::Selectable("選択なし", currentBT.empty()))
			{
				currentBT = "";
				isChanged = true;
			}
			if (currentBT.empty()) ImGui::SetItemDefaultFocus();

			for (const auto& name : behaviorTreeNames)
			{
				bool isSelected = (currentBT == name);
				if (ImGui::Selectable(name.c_str(), isSelected))
				{
					currentBT = name;
					isChanged = true;
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
		return isChanged;
	}

	/// @brief ビヘイビアツリーの設定を描画する
	/// @param btConfig 
	/// @param behaviorTreeNames 
	/// @param isDirty 
	/// @return 
	bool DrawBehaviorTreeSettings(BehaviorTreeConfig& btConfig, const std::vector<std::string>& behaviorTreeNames, bool& isDirty)
	{
		bool anyChanged = false;
		if (ImGui::CollapsingHeader("ビヘイビアツリー設定"))
		{
			if (BeginPropertyTable("BTTable"))
			{
				auto DrawRow = [&](const char* label, std::string& currentBT)
					{
						PropertyLabel(label);
						return DrawBehaviorTreeSelector(label, currentBT, behaviorTreeNames);
					};

				if (DrawRow("None (待機)", btConfig.noneStateBT)) anyChanged = true;
				if (DrawRow("Dash（ダッシュ）", btConfig.dashStateBT)) anyChanged = true;
				if (DrawRow("Grabbing（掴み）", btConfig.grabbingStateBT)) anyChanged = true;
				if (DrawRow("Grabbed（掴まれ）", btConfig.grabbedStateBT)) anyChanged = true;
				if (DrawRow("Guard（ガード）", btConfig.guardStateBT)) anyChanged = true;
				if (DrawRow("LightDamage（弱ダメージ）", btConfig.lightDamageStateBT)) anyChanged = true;
				if (DrawRow("HeavyDamage (強ダメージ)", btConfig.heavyDamageStateBT)) anyChanged = true;
				if (DrawRow("LightDamage（倒れこみ）", btConfig.downFallingStateBT)) anyChanged = true;
				if (DrawRow("DownLying（ダウン）", btConfig.downLyingStateBT)) anyChanged = true;
				if (DrawRow("DownGettingUp（起き上がり）", btConfig.downGettingUpStateBT)) anyChanged = true;
				if (DrawRow("DownStagger（ダウン怯み）", btConfig.downStaggerStateBT)) anyChanged = true;
				if (DrawRow("BlownAway（吹き飛びあがり）", btConfig.blownAwayStateBT)) anyChanged = true;
				if (DrawRow("BlownFalling（吹き飛び落下）", btConfig.blownFallingStateBT)) anyChanged = true;
				if (DrawRow("Repel（弾き）", btConfig.repelStateBT)) anyChanged = true;
				if (DrawRow("Deflect（受け流し）", btConfig.deflectStateBT)) anyChanged = true;
				if (DrawRow("Repelled（弾かれ）", btConfig.repelledStateBT)) anyChanged = true;
				if (DrawRow("Deflected（受け流され）", btConfig.deflectedStateBT)) anyChanged = true;
				if (DrawRow("Avoid（回避）", btConfig.avoidStateBT)) anyChanged = true;
				if (DrawRow("Dead（死亡）", btConfig.deadStateBT)) anyChanged = true;

				EndPropertyTable();
			}
		}

		if (anyChanged) isDirty = true;
		return anyChanged;
	}

	/// @brief コンボツリーのセレクターを描画する
	/// @param label 
	/// @param currentCT 
	/// @param comboTreeNames 
	/// @return 
	bool DrawComboTreeSelector(const char* label, ComboTreeInputName& currentCT, const std::vector<std::string>& comboTreeNames)
	{
		bool isChanged = false;

		if (ImGui::TreeNode(label))
		{
			// X入力のセレクター
			const char* previewValueX = currentCT.xName_.empty() ? "選択なし" : currentCT.xName_.c_str();
			if (ImGui::BeginCombo("X入力", previewValueX))
			{
				// 選択なしのオプションを追加
				if (ImGui::Selectable("選択なし", currentCT.xName_.empty()))
				{
					currentCT.xName_ = "";
					isChanged = true;
				}
				if (currentCT.xName_.empty()) ImGui::SetItemDefaultFocus();

				for (const auto& name : comboTreeNames)
				{
					bool isSelected = (currentCT.xName_ == name);
					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						currentCT.xName_ = name;
						isChanged = true;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			// Y入力のセレクター
			const char* previewValueY = currentCT.yName_.empty() ? "選択なし" : currentCT.yName_.c_str();
			if (ImGui::BeginCombo("Y入力", previewValueY))
			{
				// 選択なしのオプションを追加
				if (ImGui::Selectable("選択なし", currentCT.yName_.empty()))
				{
					currentCT.yName_ = "";
					isChanged = true;
				}
				if (currentCT.yName_.empty()) ImGui::SetItemDefaultFocus();

				for (const auto& name : comboTreeNames)
				{
					bool isSelected = (currentCT.yName_ == name);
					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						currentCT.yName_ = name;
						isChanged = true;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			// B入力のセレクター
			const char* previewValueB = currentCT.bName_.empty() ? "選択なし" : currentCT.bName_.c_str();
			if (ImGui::BeginCombo("B入力", previewValueB))
			{
				// 選択なしのオプションを追加
				if (ImGui::Selectable("選択なし", currentCT.bName_.empty()))
				{
					currentCT.bName_ = "";
					isChanged = true;
				}
				if (currentCT.bName_.empty()) ImGui::SetItemDefaultFocus();

				for (const auto& name : comboTreeNames)
				{
					bool isSelected = (currentCT.bName_ == name);
					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						currentCT.bName_ = name;
						isChanged = true;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}


			ImGui::TreePop();
		}

		return isChanged;
	}

	/// @brief コンボツリーの設定を描画する
	/// @param ctConfig 
	/// @param comboTreeNames 
	/// @param isDirty 
	/// @return 
	bool DrawComboTreeSettings(ComboTreeConfig& ctConfig, const std::vector<std::string>& comboTreeNames, bool& isDirty)
	{
		bool anyChanged = false;
		if (ImGui::CollapsingHeader("コンボツリー設定"))
		{
			if (BeginPropertyTable("BTTable"))
			{
				auto DrawRow = [&](const char* label, ComboTreeInputName& currentCT)
					{
						PropertyLabel(label);
						return DrawComboTreeSelector(label, currentCT, comboTreeNames);
					};

				if (DrawRow("None (待機)", ctConfig.noneStateCT)) anyChanged = true;
				if (DrawRow("Dash（ダッシュ）", ctConfig.dashStateCT)) anyChanged = true;
				if (DrawRow("Grabbing（掴み）", ctConfig.grabbingStateCT)) anyChanged = true;
				if (DrawRow("Grabbed（掴まれ）", ctConfig.grabbedStateCT)) anyChanged = true;
				if (DrawRow("Guard（ガード）", ctConfig.guardStateCT)) anyChanged = true;
				if (DrawRow("LightDamage（弱ダメージ）", ctConfig.lightDamageStateCT)) anyChanged = true;
				if (DrawRow("HeavyDamage (強ダメージ)", ctConfig.heavyDamageStateCT)) anyChanged = true;
				if (DrawRow("LightDamage（倒れこみ）", ctConfig.downFallingStateCT)) anyChanged = true;
				if (DrawRow("DownLying（ダウン）", ctConfig.downLyingStateCT)) anyChanged = true;
				if (DrawRow("DownGettingUp（起き上がり）", ctConfig.downGettingUpStateCT)) anyChanged = true;
				if (DrawRow("DownStagger（ダウン怯み）", ctConfig.downStaggerStateCT)) anyChanged = true;
				if (DrawRow("BlownAway（吹き飛びあがり）", ctConfig.blownAwayStateCT)) anyChanged = true;
				if (DrawRow("BlownFalling（吹き飛び落下）", ctConfig.blownFallingStateCT)) anyChanged = true;
				if (DrawRow("Repel（弾き）", ctConfig.repelStateCT)) anyChanged = true;
				if (DrawRow("Deflect（受け流し）", ctConfig.deflectStateCT)) anyChanged = true;
				if (DrawRow("Repelled（弾かれ）", ctConfig.repelledStateCT)) anyChanged = true;
				if (DrawRow("Deflected（受け流され）", ctConfig.deflectedStateCT)) anyChanged = true;
				if (DrawRow("Avoid（回避）", ctConfig.avoidStateCT)) anyChanged = true;
				if (DrawRow("Dead（死亡）", ctConfig.deadStateCT)) anyChanged = true;

				EndPropertyTable();
			}
		}

		if (anyChanged) isDirty = true;
		return anyChanged;
	}

	/// @brief モーションのセレクターを描画する
	/// @param label 
	/// @param motionType 
	/// @param motionConfig 
	/// @param motionManager 
	/// @return 
	bool DrawMotionSelector(const char* label, MotionType motionType, MotionConfig& motionConfig, MotionManager* motionManager)
	{
		std::vector<std::string> motionNames = motionManager->GetMotionNames(motionType);
		bool isChanged = false;

		if (motionNames.empty())
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
		}
		else
		{
			const char* previewValue = motionConfig.name.empty() ? "モーションを選択..." : motionConfig.name.c_str();
			if (ImGui::BeginCombo(label, previewValue))
			{
				for (const auto& name : motionNames)
				{
					bool isSelected = (motionConfig.name == name);
					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						motionConfig.name = name;
						motionConfig.handle = motionManager->GetMotion(motionType, motionConfig.name);
						isChanged = true;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		return isChanged;
	}

	/// @brief モーションのセレクターを描画する（履歴対応版）
	/// @param label 
	/// @param motionType 
	/// @param motionConfig 
	/// @param placementList 
	/// @param isDirty 
	/// @param history 
	/// @param motionManager 
	/// @return 
	bool DrawMotionSelectorWithHistory(const char* label, MotionType motionType, MotionConfig& motionConfig,
		std::vector<PlacementData>& placementList, bool& isDirty, StageEditorHistory* history, MotionManager* motionManager)
	{
		std::vector<std::string> motionNames = motionManager->GetMotionNames(motionType);
		bool isChanged = false;

		if (motionNames.empty())
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
		}
		else
		{
			const char* previewValue = motionConfig.name.empty() ? "モーションを選択..." : motionConfig.name.c_str();
			if (ImGui::BeginCombo(label, previewValue))
			{
				for (const auto& name : motionNames)
				{
					bool isSelected = (motionConfig.name == name);
					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						history->SaveHistory(placementList);
						isDirty = true;

						motionConfig.name = name;
						motionConfig.handle = motionManager->GetMotion(motionType, motionConfig.name);
						isChanged = true;
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		return isChanged;
	}

	/// @brief キャラクターのモーション設定を描画する
	/// @param target 
	/// @param placementList 
	/// @param isDirty 
	/// @param history 
	/// @param motionManager 
	/// @param useHistory 
	/// @return 
	bool DrawCharacterMotionSettings(PlacementData& target, std::vector<PlacementData>& placementList,
		bool& isDirty, StageEditorHistory* history, MotionManager* motionManager, bool useHistory)
	{
		bool anyChanged = false;

		if (ImGui::CollapsingHeader("モーション設定"))
		{
			if (BeginPropertyTable("MotionTable"))
			{
				// ラベルとモーションセレクターを描画するラムダ関数
				auto DrawRow = [&](const char* label, MotionType type, MotionConfig& config)
					{
						PropertyLabel(label);
						if (useHistory) {
							return DrawMotionSelectorWithHistory(label, type, config, placementList, isDirty, history, motionManager);
						}
						else {
							return DrawMotionSelector(label, type, config, motionManager);
						}
					};

				// ラベル付きで一行ずつ描画 (Combo内のラベルは空文字にして、PropertyLabelに任せる)
				if (DrawRow("待機モーション", MotionType::Stand, target.standMotion)) anyChanged = true;
				if (DrawRow("戦闘モーション", MotionType::Stance, target.stanceMotion)) anyChanged = true;
				if (DrawRow("歩行モーション", MotionType::Walk, target.walkMotion)) anyChanged = true;
				if (DrawRow("ダッシュモーション", MotionType::Dash, target.dashMotion)) anyChanged = true;
				if (DrawRow("前方回避モーション", MotionType::Avoid, target.avoidFrontMotion)) anyChanged = true;
				if (DrawRow("後方回避モーション", MotionType::Avoid, target.avoidBackMotion)) anyChanged = true;
				if (DrawRow("左回避モーション", MotionType::Avoid, target.avoidLeftMotion)) anyChanged = true;
				if (DrawRow("右回避モーション", MotionType::Avoid, target.avoidRightMotion)) anyChanged = true;
				if (DrawRow("防御モーション", MotionType::Guard, target.guardMotion)) anyChanged = true;

				EndPropertyTable();
			}
		}
		return anyChanged;
	}

	/// @brief キャラクターの基本設定を描画する
	/// @param target 
	/// @param placementList 
	/// @param isDirty 
	/// @param history 
	/// @param useHistory 
	void DrawCharacterBaseSettings(PlacementData& target, std::vector<PlacementData>& placementList,
		bool& isDirty, StageEditorHistory* history, bool useHistory)
	{
		if (target.instancePtr == nullptr)
		{
			ImGui::TextColored(ImVec4(1, 0, 0, 1), "キャラクターの実体が存在しません。配置を再生成してください。");
			return;
		}

		if (ImGui::CollapsingHeader("基本ステータス", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (BeginPropertyTable("CharBaseTable"))
			{
				// キャラクターの実体ポインタを取得
				Character* charPtr = static_cast<Character*>(target.instancePtr);

				// 位置情報
				PropertyLabel("生成位置");
				if (ImGui::DragFloat3("##Pos", &target.position.x, 0.1f))
				{
					isDirty = true;
				}

				// 回転情報
				PropertyLabel("回転 (Y軸)");
				if (ImGui::DragFloat("##RotY", &target.rotate_.y, 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>))
				{
					isDirty = true;
				}

				// HP
				PropertyLabel("HP");
				if (ImGui::DragInt("##HP", &target.hp, 1, 0, charPtr->GetMaxHp()))
				{
					
					isDirty = true;
					charPtr->SetHp(target.hp);
				}

				// NPCの場合は攻撃性を設定できるようにする
				if (target.subType != static_cast<int32_t>(CharacterTag::Player))
				{
					NPC* npcPtr = static_cast<NPC*>(target.instancePtr);

					// 攻撃性
					PropertyLabel("攻撃性");
					if (ImGui::DragFloat("##Aggressiveness", &target.aggressiveness, 0.1f, 0.0f, 100.0f))
					{
						npcPtr->SetAggressiveness(target.aggressiveness);
						isDirty = true;
					}
				}

				EndPropertyTable();
			}
		}
	}

	/// @brief イベントトリガーの設定を描画する
	/// @param target 
	/// @param placementList 
	/// @param isDirty 
	/// @param history 
	/// @param spawner 
	/// @param scene 
	/// @param eventStageDataFileNames 
	/// @param cutsceneNames 
	void DrawEventTriggerSettings(PlacementData& target, std::vector<PlacementData>& placementList,
		bool& isDirty, StageEditorHistory* history, StageSpawner* spawner, GameScene* scene,
		const std::vector<std::string>& eventStageDataFileNames, const std::vector<std::string>& cutsceneNames)
	{
		StaticEventTrigger* eventTriggerPtr = static_cast<StaticEventTrigger*>(target.instancePtr);

		ImGui::Unindent();
		ImGui::Separator();
		ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "--- イベントトリガー設定 ---");
		ImGui::Indent();

		int currentType = target.eventType;

		if (ImGui::Combo("イベントタイプ", &currentType, eventTypeNames, IM_ARRAYSIZE(eventTypeNames)))
		{
			if (eventTriggerPtr != nullptr)
			{
				history->SaveHistory(placementList);
				isDirty = true;

				target.eventType = currentType;
				eventTriggerPtr->SetEventType(currentType);

				// 配置されている実体を再生成して反映する
				spawner->SpawnActualEntity(target);
			}
			else
			{
				target.eventType = currentType;
			}
		}

		if (target.eventType == static_cast<int32_t>(StaticEventTrigger::EventType::ObjectSpawn))
		{
			ImGui::Text("ステージデータの設定");

			std::string currentSdName = target.eventStageDataFileName;
			const char* previewSdValue = currentSdName.empty() ? "ステージデータを選択..." : currentSdName.c_str();

			if (ImGui::BeginCombo("ステージデータ", previewSdValue))
			{
				for (const auto& name : eventStageDataFileNames)
				{
					bool isSelected = (currentSdName == name);
					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						if (eventTriggerPtr != nullptr)
						{
							history->SaveHistory(placementList);
							isDirty = true;
						}
						strcpy_s(target.eventStageDataFileName, sizeof(target.eventStageDataFileName), name.c_str());
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		else if (target.eventType == static_cast<int32_t>(StaticEventTrigger::EventType::PlayCutscene))
		{
			std::vector<std::string> cutsceneNames = scene->GetCutsceneManager()->GetCutsceneNames();

			std::string currentCutsceneName = target.eventCutsceneName;
			const char* previewCutsceneValue = currentCutsceneName.empty() ? "カットシーンを選択..." : currentCutsceneName.c_str();

			if (ImGui::BeginCombo("カットシーン", previewCutsceneValue))
			{
				for (const auto& name : cutsceneNames)
				{
					bool isSelected = (currentCutsceneName == name);
					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						if (eventTriggerPtr != nullptr)
						{
							history->SaveHistory(placementList);
							isDirty = true;
						}
						strcpy_s(target.eventCutsceneName, sizeof(target.eventCutsceneName), name.c_str());
					}
					if (isSelected) ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
	}
}