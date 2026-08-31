#include "StageEditorUIHelper.h"
#include <numbers>
#include "StageEditor/StageSpawner/StageSpawner.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"
#include "Scene/GameScene/GameScene.h"
#include "StageObject/StaticEventTrigger/StaticEventTrigger.h"

// プレハブデータの保存先フォルダ
constexpr const char* kPrefabDir = "./Assets/Parameter/EditorPrefab/";

namespace StageEditorUIHelper
{
	/// @brief Prefabの名前を取得する
	/// @return 
	std::vector<std::string> GetPrefabNames()
	{
		std::vector<std::string> names;
		if (!std::filesystem::exists(kPrefabDir))
		{
			std::filesystem::create_directories(kPrefabDir);
			return names;
		}

		for (const auto& entry : std::filesystem::directory_iterator(kPrefabDir))
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
	void SavePrefab(const std::string& name, const TemplateData& data)
	{
		std::filesystem::create_directories(kPrefabDir);
		std::string filePath = std::string(kPrefabDir) + name + ".json";

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
	bool LoadPrefab(const std::string& name, TemplateData& data)
	{
		std::string filePath = std::string(kPrefabDir) + name + ".json";
		std::ifstream file(filePath);
		if (file.is_open())
		{
			json j;
			file >> j;
			fromJson(j, data);
			file.close();

			return true;
		}

		return false;
	}

	/// @brief テーブルレイアウトの開始
	/// @param strId 
	/// @return 
	bool BeginPropertyTable(const char* strId)
	{
		// 2列のテーブルを作成（1列目はラベル用に幅固定、2列目は自動ストレッチ）
		if (ImGui::BeginTable(strId, 2, ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_RowBg))
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
	/// @param motionManager 
	/// @return 
	bool DrawCharacterMotionSettings(TemplateData& target,MotionManager* motionManager)
	{
		bool anyChanged = false;

		if (ImGui::CollapsingHeader("モーション設定"))
		{
			if (BeginPropertyTable("MotionTable"))
			{
				// ラベル付きで一行ずつ描画するラムダ関数を定義
				auto DrawRow = [&](const char* label, MotionType type, MotionConfig& config)
					{
						PropertyLabel(label);
						return DrawMotionSelector(label, type, config, motionManager);
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
	void DrawCharacterPlacementSettings(PlacementData& target, bool& isDirty)
	{
		// キャラクターの実体ポインタを取得
		Character* charPtr = target.instancePtr.type() == typeid(Character*) ? std::any_cast<Character*>(target.instancePtr) : nullptr;

		if (ImGui::CollapsingHeader("基本ステータス", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (BeginPropertyTable("CharBaseTable"))
			{
				// 位置情報
				PropertyLabel("生成位置");
				if (ImGui::DragFloat3("##Pos", &target.position.x, 0.1f))
				{
					isDirty = true;
					if (charPtr)
						charPtr->SetPosition(target.position);
				}

				// 回転情報
				PropertyLabel("回転 (Y軸)");
				if (ImGui::DragFloat("##RotY", &target.rotate_.y, 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>))
				{
					isDirty = true;
					if (charPtr)
						charPtr->SetRotation(target.rotate_);
				}

				EndPropertyTable();
			}
		}
	}

	/// @brief キャラクターのテンプレート設定を描画する
	/// @param target 
	/// @param isDirty 
	void DrawCharacterTemplateSettings(TemplateData& target, bool& isDirty)
	{
		if (ImGui::CollapsingHeader("基本ステータス", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (BeginPropertyTable("CharBaseTable"))
			{
				// HP
				PropertyLabel("HP");
				if (ImGui::DragInt("##HP", &target.hp, 1, 0, 1000000))
				{
					isDirty = true;
				}

				// ガードゲージ量
				PropertyLabel("ガードゲージ量");
				if (ImGui::DragFloat("##ガードゲージ量", &target.guardGage, 0.01f, 0.0f, 100000.0f))
				{
					isDirty = true;
				}

				// ガード回復時間
				PropertyLabel("ガード回復時間");
				if (ImGui::DragFloat("##ガード回復時間", &target.guardRecoveryTime, 0.01f, 0.0f, 100000.0f))
				{
					isDirty = true;
				}

				// ほどき時間
				PropertyLabel("ほどき時間");
				if (ImGui::DragFloat("##ほどき時間", &target.unravellingTime, 0.01f, 0.0f, 100000.0f))
				{
					isDirty = true;
				}

				// NPCの場合は攻撃性を設定できるようにする
				if (target.subType != static_cast<int32_t>(CharacterTag::Player))
				{
					// 攻撃性
					PropertyLabel("攻撃性");
					if (ImGui::DragFloat("##Aggressiveness", &target.aggressiveness, 0.1f, 0.0f, 100.0f))
					{
						isDirty = true;
					}
				}

				EndPropertyTable();
			}
		}


		ImGui::Separator();
		ImGui::Text("初期装備武器");

		// プレハブ一覧を取得
		std::vector<std::string> prefabNames = StageEditorUIHelper::GetPrefabNames();
		std::string currentWeapon = target.equipWeaponPrefabName;

		if (ImGui::BeginCombo("武器プレハブ", currentWeapon.empty() ? "なし" : currentWeapon.c_str()))
		{
			// 「なし」を選択できるようにする
			if (ImGui::Selectable("なし", currentWeapon.empty()))
			{
				memset(target.equipWeaponPrefabName, 0, sizeof(target.equipWeaponPrefabName));
				isDirty = true;
			}

			for (const auto& prefabName : prefabNames)
			{
				bool isSelected = (currentWeapon == prefabName);
				if (ImGui::Selectable(prefabName.c_str(), isSelected))
				{
					strncpy_s(target.equipWeaponPrefabName, prefabName.c_str(), sizeof(target.equipWeaponPrefabName) - 1);
					isDirty = true;
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		MotionManager* motionManager = MotionManager::GetInstance();

		// モーション設定UIを描画し、変更があったかどうかを取得
		bool motionChanged =
			StageEditorUIHelper::DrawCharacterMotionSettings(target, motionManager);

		// もしモーションのどれかが変更されたら、実際のキャラクターオブジェクトにアニメーションハンドルを更新する
		if (motionChanged)
		{
			AnimationHandleData animationData;
			animationData.hStandMotion = motionManager->GetMotion(MotionType::Stand, target.standMotion.name);
			animationData.hStanceMotion = motionManager->GetMotion(MotionType::Stance, target.stanceMotion.name);
			animationData.hWalkMotion = motionManager->GetMotion(MotionType::Walk, target.walkMotion.name);
			animationData.hDashMotion = motionManager->GetMotion(MotionType::Dash, target.dashMotion.name);
			animationData.hAvoidFrontMotion = motionManager->GetMotion(MotionType::Avoid, target.avoidFrontMotion.name);
			animationData.hAvoidBackMotion = motionManager->GetMotion(MotionType::Avoid, target.avoidBackMotion.name);
			animationData.hAvoidLeftMotion = motionManager->GetMotion(MotionType::Avoid, target.avoidLeftMotion.name);
			animationData.hAvoidRightMotion = motionManager->GetMotion(MotionType::Avoid, target.avoidRightMotion.name);
			animationData.hGuardMotion = motionManager->GetMotion(MotionType::Guard, target.guardMotion.name);
		}
	}

	/// @brief 武器の基本設定を描画する
	/// @param target 
	/// @param isDirty 
	void DrawWeaponPlacementSettings(PlacementData& target, bool& isDirty)
	{
		if (ImGui::CollapsingHeader("基本ステータス", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (BeginPropertyTable("CharBaseTable"))
			{
				// キャラクターの実体ポインタを取得
				Weapon* weaponPtr = target.instancePtr.type() == typeid(Weapon*) ? std::any_cast<Weapon*>(target.instancePtr) : nullptr;

				// 位置情報
				PropertyLabel("位置");
				if (ImGui::DragFloat3("##位置", &target.position.x, 0.1f))
				{
					isDirty = true;
					if (weaponPtr)
						weaponPtr->SetPosition(target.position);
				}

				EndPropertyTable();
			}
		}
	}

	/// @brief 武器のテンプレート設定を描画する
	/// @param target 
	/// @param isDirty 
	void DrawWeaponTemplateSettings(TemplateData& target, bool& isDirty)
	{
		if (ImGui::CollapsingHeader("基本ステータス", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (BeginPropertyTable("CharBaseTable"))
			{
				// 耐久力
				PropertyLabel("耐久力");
				if (ImGui::DragInt("##Durability", &target.durability, 1, 0, 1000000))
				{
					isDirty = true;
				}

				// 攻撃力
				PropertyLabel("攻撃力");
				if (ImGui::DragFloat("##Attack", &target.attackPower, 0.01f, 0.0f, 100000.0f))
				{
					isDirty = true;
				}

				// 壊れない武器かどうか
				PropertyLabel("壊れない武器");
				if (ImGui::Checkbox("##Unbreakable", &target.isUnbreakable))
				{
					isDirty = true;
				}

				EndPropertyTable();
			}
		}
	}

	/// @brief ステージオブジェクトの基本設定を描画する
	/// @param target 
	/// @param placementList 
	/// @param isDirty 
	/// @param history 
	/// @param useHistory 
	void DrawStageObjectPlacementSettings(PlacementData& target, bool& isDirty)
	{
		if (ImGui::CollapsingHeader("基本ステータス", ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (BeginPropertyTable("CharBaseTable"))
			{
				// キャラクターの実体ポインタを取得
				StageObject* stageObjectPtr = target.instancePtr.type() == typeid(StageObject*) ? std::any_cast<StageObject*>(target.instancePtr) : nullptr;

				// 位置情報
				PropertyLabel("位置");
				if (ImGui::DragFloat3("##位置", &target.position.x, 0.1f))
				{
					isDirty = true;
					if(stageObjectPtr)
						stageObjectPtr->SetPosition(target.position);
				}

				// 回転情報
				if (target.subType != static_cast<int>(StageObject::StageObjectTag::Floor) && target.subType != static_cast<int>(StageObject::StageObjectTag::StaticEventTrigger))
				{
					PropertyLabel("回転");
					if (ImGui::DragFloat3("##回転", &target.rotate_.x, 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>))
					{
						isDirty = true;
						if (stageObjectPtr)
							stageObjectPtr->SetRotation(target.rotate_);
					}
				}

				// 拡縮
				PropertyLabel("拡縮");
				if (ImGui::DragFloat3("##Scale", &target.scale.x, 0.01f, 0.0f, 100.0f))
				{
					isDirty = true;
					if (stageObjectPtr)
						stageObjectPtr->SetScale(target.scale);
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
	void DrawEventTriggerSettings(PlacementData& target, bool& isDirty, GameScene* scene,
		const std::vector<std::string>& eventStageDataFileNames, const std::vector<std::string>& cutsceneNames)
	{
		ImGui::Unindent();
		ImGui::Separator();
		ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "--- イベントトリガー設定 ---");
		ImGui::Indent();

		int currentType = target.eventType;

		if (ImGui::Combo("イベントタイプ", &currentType, eventTypeNames, IM_ARRAYSIZE(eventTypeNames)))
		{
			isDirty = true;
			target.eventType = currentType;
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
						isDirty = true;
						strcpy_s(target.eventStageDataFileName, sizeof(target.eventStageDataFileName), name.c_str());
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		else if (target.eventType == static_cast<int32_t>(StaticEventTrigger::EventType::PlayCutscene))
		{
			std::vector<std::string> cutsceneNamesList = scene->GetCutsceneManager()->GetCutsceneNames();

			std::string currentCutsceneName = target.eventCutsceneName;
			const char* previewCutsceneValue = currentCutsceneName.empty() ? "カットシーンを選択..." : currentCutsceneName.c_str();

			if (ImGui::BeginCombo("カットシーン", previewCutsceneValue))
			{
				for (const auto& name : cutsceneNamesList)
				{
					bool isSelected = (currentCutsceneName == name);
					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						isDirty = true;
						strcpy_s(target.eventCutsceneName, sizeof(target.eventCutsceneName), name.c_str());
					}
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
		}
		else if (target.eventType == static_cast<int32_t>(StaticEventTrigger::EventType::NavMeshStateChange))
		{
			ImGui::Separator();
			ImGui::Text("--- ナビメッシュ切り替え設定 ---");

			// グループIDの入力
			if (ImGui::InputInt("対象グループID", &target.targetNavMeshGroupId))
			{
				isDirty = true;
			}

			// 有効/無効の切り替え
			if (ImGui::Checkbox("切り替え後の状態 (チェックで有効)", &target.targetNavMeshState))
			{
				isDirty = true;
			}
		}
	}

	/// @brief 配置データのテンプレートを変更する
	/// @param target 
	/// @param newTemplateName 
	/// @param spawner 
	/// @param isDirty 
	void ChangePlacementTemplate(PlacementData& target, const std::string& newTemplateName, StageSpawner* spawner)
	{
		// 画面上に存在する既存のオブジェクト実体を削除
		spawner->DeleteActualEntity(target);

		// テンプレート名を更新
		strncpy_s(target.templateName, newTemplateName.c_str(), sizeof(target.templateName) - 1);

		// 該当するテンプレートファイル（.json）をロード
		TemplateData tempData;
		if (StageEditorUIHelper::LoadPrefab(newTemplateName, tempData))
		{
			// テンプレートの情報（カテゴリやサブタイプなど）を配置データに反映
			target.category = tempData.category;
			target.subType = tempData.subType;

			// 新しいテンプレート情報で実体を再生成
			spawner->SpawnActualEntity(target);
		}
	}
}