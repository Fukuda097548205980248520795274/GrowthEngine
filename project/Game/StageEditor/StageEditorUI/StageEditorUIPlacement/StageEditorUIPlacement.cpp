#include "StageEditorUIPlacement.h" 
#include "StageEditor/StageSpawner/StageSpawner.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"
#include <numbers>

#include "Entity/Character/NPC/NPC.h"
#include "Entity/Character/Player/Player.h"
#include "Entity/Weapon/Weapon.h"

#include "StageObject/Floor/Floor.h"
#include "StageObject/Wall/Wall.h"
#include "StageObject/StaticEventTrigger/StaticEventTrigger.h"

#include "Scene/GameScene/GameScene.h"

/// @brief コンストラクタ
/// @param spawner 
/// @param history 
StageEditorUIPlacement::StageEditorUIPlacement(StageSpawner* spawner, StageEditorHistory* history, GameScene* scene)
	: spawner_(spawner), history_(history), scene_(scene)
{ 
	// モーションマネージャのインスタンスを取得
	motionManager_ = MotionManager::GetInstance();
}

/// @brief UIの描画
/// @param placementList 
/// @param selectedIndex 
/// @param isDirty 
/// @param isPlaying
void StageEditorUIPlacement::DrawUI(std::vector<PlacementData>& placementList, int& selectedIndex, bool& isDirty, 
	const std::vector<std::string>& behaviorTreeNames, const std::vector<std::string>& eventStageDataFileNames)
{
	// オブジェクト配置モードのUIを描画
	ImGui::Text("--- オブジェクト配置 ---");


	// ビヘイビアツリーの選択UIを描画するラムダ関数
	auto DrawBehaviorTreeSelector = [&](const char* label, std::string& currentBT) -> bool
		{
			bool isChanged = false;
			const char* previewValue = currentBT.empty() ? "None" : currentBT.c_str();

			if (ImGui::BeginCombo(label, previewValue))
			{
				// behaviorTreeNames は既存のリストを利用
				for (const auto& name : behaviorTreeNames)
				{
					bool isSelected = (currentBT == name);
					if (ImGui::Selectable(name.c_str(), isSelected))
					{
						currentBT = name;
						isChanged = true;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
			return isChanged;
		};


	static PlacementData currentData;
	static bool isInitialized = false;
	if (!isInitialized)
	{
		currentData.category = EditCategory::Character;
		currentData.subType = 0;
		currentData.position = Vector3(0.0f, 0.0f, 0.0f);
		currentData.rotate_ = Vector3(0.0f, 0.0f, 0.0f);
		currentData.scale = Vector3(1.0f, 1.0f, 1.0f);
		currentData.hp = 100;
		currentData.durability = 100;
		currentData.attackPower = 1.0f;
		currentData.isUnbreakable = false;
		currentData.eventType = 0;
		currentData.eventStageDataFileName[0] = '\0';
		currentData.standMotion.name = "Standing";
		currentData.stanceMotion.name = "Fighter";
		currentData.walkMotion.name = "Walk";
		currentData.dashMotion.name = "Dash";
		currentData.avoidFrontMotion.name = "Front";
		currentData.avoidBackMotion.name = "Back";
		currentData.avoidLeftMotion.name = "Front";
		currentData.avoidRightMotion.name = "Back";
		currentData.guardMotion.name = "BothHands";

		isInitialized = true;
	}

	// 大分類の選択
	int intCat = static_cast<int>(currentData.category);
	if (ImGui::Combo("大分類", &intCat, categoryNames, IM_ARRAYSIZE(categoryNames)))
	{
		currentData.category = static_cast<EditCategory>(intCat);
		currentData.subType = 0; // 大分類が変わったら小分類のリセット
	}

	ImGui::Separator();

	// 大分類に応じて、小分類のコンボボックスの中身を切り替える
	if (currentData.category == EditCategory::Character)
	{
		ImGui::Combo("キャラクターの種類", &currentData.subType, characterTagNames, IM_ARRAYSIZE(characterTagNames));

		// 位置
		ImGui::DragFloat3("生成位置", &currentData.position.x, 0.1f);

		// HP
		ImGui::DragInt("HP", &currentData.hp, 1, 0, 10000);

		// 回転
		ImGui::DragFloat("回転", &currentData.rotate_.y, 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>);

		// もしNPCが選ばれていたら、モーションの選択UIも表示する
		MotionSelecter("立ちモーション", MotionType::Stand, currentData.standMotion);
		MotionSelecter("戦闘モーション", MotionType::Stance, currentData.stanceMotion);
		MotionSelecter("歩行モーション", MotionType::Walk, currentData.walkMotion);
		MotionSelecter("ダッシュモーション", MotionType::Dash, currentData.dashMotion);
		MotionSelecter("前方回避モーション", MotionType::Avoid, currentData.avoidFrontMotion);
		MotionSelecter("後方回避モーション", MotionType::Avoid, currentData.avoidBackMotion);
		MotionSelecter("左回避モーション", MotionType::Avoid, currentData.avoidLeftMotion);
		MotionSelecter("右回避モーション", MotionType::Avoid, currentData.avoidRightMotion);
		MotionSelecter("防御モーション", MotionType::Guard, currentData.guardMotion);

		// プレイヤーと未選択以外　ビヘイビアツリーデータ
		if (currentData.subType != 0 && currentData.subType != 1)
		{
			ImGui::Separator();
			if (ImGui::CollapsingHeader("ビヘイビアツリー設定"))
			{
				if (DrawBehaviorTreeSelector("None (待機)", currentData.behaviorTrees.noneStateBT)) isDirty = true;
				if (DrawBehaviorTreeSelector("Dash（ダッシュ）", currentData.behaviorTrees.dashStateBT)) isDirty = true;

				if (DrawBehaviorTreeSelector("Grabbing（掴み）", currentData.behaviorTrees.grabbingStateBT)) isDirty = true;
				if (DrawBehaviorTreeSelector("Grabbed（掴まれ）", currentData.behaviorTrees.grabbedStateBT)) isDirty = true;
				if (DrawBehaviorTreeSelector("Guard（ガード）", currentData.behaviorTrees.guardStateBT)) isDirty = true;

				if (DrawBehaviorTreeSelector("LightDamage（弱ダメージ）", currentData.behaviorTrees.lightDamageStateBT)) isDirty = true;
				if (DrawBehaviorTreeSelector("HeavyDamage (強ダメージ)", currentData.behaviorTrees.heavyDamageStateBT)) isDirty = true;

				if (DrawBehaviorTreeSelector("LightDamage（倒れこみ）", currentData.behaviorTrees.downFallingStateBT)) isDirty = true;
				if (DrawBehaviorTreeSelector("DownLying（ダウン）", currentData.behaviorTrees.downLyingStateBT)) isDirty = true;
				if (DrawBehaviorTreeSelector("DownGettingUp（起き上がり）", currentData.behaviorTrees.downGettingUpStateBT)) isDirty = true;
				if (DrawBehaviorTreeSelector("DownStagger（ダウン怯み）", currentData.behaviorTrees.downStaggerStateBT)) isDirty = true;
				if (DrawBehaviorTreeSelector("BlownAway（吹き飛びあがり）", currentData.behaviorTrees.blownAwayStateBT)) isDirty = true;
				if (DrawBehaviorTreeSelector("BlownFalling（吹き飛び落下）", currentData.behaviorTrees.blownFallingStateBT)) isDirty = true;

				if (DrawBehaviorTreeSelector("Repel（弾き）", currentData.behaviorTrees.repelStateBT)) isDirty = true;
				if (DrawBehaviorTreeSelector("Deflect（受け流し）", currentData.behaviorTrees.deflectStateBT)) isDirty = true;

				if (DrawBehaviorTreeSelector("Repelled（弾かれ）", currentData.behaviorTrees.repelledStateBT)) isDirty = true;
				if (DrawBehaviorTreeSelector("Deflected（受け流され）", currentData.behaviorTrees.deflectedStateBT)) isDirty = true;

				if (DrawBehaviorTreeSelector("Avoid（回避）", currentData.behaviorTrees.avoidStateBT)) isDirty = true;
				if (DrawBehaviorTreeSelector("Dead（死亡）", currentData.behaviorTrees.deadStateBT)) isDirty = true;
			}
		}
	}
	else if (currentData.category == EditCategory::Object)
	{
		ImGui::Combo("オブジェクトの種類", &currentData.subType, stageObjectTagNames, IM_ARRAYSIZE(stageObjectTagNames));

		// 床
		if (static_cast<StageObject::StageObjectTag>(currentData.subType) == StageObject::StageObjectTag::Floor)
		{
			// 位置
			ImGui::DragFloat3("生成位置", &currentData.position.x, 0.1f);

			// 拡縮
			ImGui::DragFloat3("大きさ", &currentData.scale.x, 0.1f, 0.0f, 10000.0f);
		} 
		else if (static_cast<StageObject::StageObjectTag>(currentData.subType) == StageObject::StageObjectTag::Wall)
		{
			// 位置
			ImGui::DragFloat3("生成位置", &currentData.position.x, 0.1f);

			// 回転
			ImGui::DragFloat("回転Y", &currentData.rotate_.y, 0.01f, -std::numbers::pi_v<float>, std::numbers::pi_v<float>);

			// 拡縮
			ImGui::DragFloat3("大きさ", &currentData.scale.x, 0.1f, 0.0f, 10000.0f);
		}
		else if (static_cast<StageObject::StageObjectTag>(currentData.subType) == StageObject::StageObjectTag::StaticEventTrigger)
		{
			// 位置
			ImGui::DragFloat3("生成位置", &currentData.position.x, 0.1f);

			// 拡縮
			ImGui::DragFloat3("大きさ", &currentData.scale.x, 0.1f, 0.0f, 10000.0f);

			// イベントの種類
			ImGui::Combo("イベントタイプ", &currentData.eventType, eventTypeNames, IM_ARRAYSIZE(eventTypeNames));
			ImGui::Separator();

			// イベントタイプが「オブジェクト生成」の場合、ステージデータの設定UIを表示する
			if (currentData.eventType == 1)
			{
				ImGui::Text("ステージデータの設定");

				// プレビュー用の文字列（未設定の場合は "ステージデータを選択..." と表示）
				std::string currentSdName = currentData.eventStageDataFileName;
				const char* previewSdValue = currentSdName.empty() ? "ステージデータを選択..." : currentSdName.c_str();

				// プルダウンメニュー（コンボボックス）の描画
				if (ImGui::BeginCombo("ステージデータ", previewSdValue))
				{
					for (const auto& name : eventStageDataFileNames)
					{
						// 現在のステージデータ名と同じものが選択されている状態にする
						bool isSelected = (currentSdName == name);
						if (ImGui::Selectable(name.c_str(), isSelected))
						{
							// ビヘイビアツリーを変更する前に、現在の配置リストの状態を履歴に保存する
							history_->SaveHistory(placementList);
							isDirty = true;

							// 選択された名前を PlacementData の配列にコピーする
							strcpy_s(currentData.eventStageDataFileName, sizeof(currentData.eventStageDataFileName), name.c_str());
						}

						// 選択中のアイテムにフォーカスを合わせる
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}
			}
			else if (currentData.eventType == 2)
			{
				// GameScene経由で登録されている演出名一覧を取得
				std::vector<std::string> cutsceneNames = scene_->GetCutsceneManager()->GetCutsceneNames();

				// プレビュー用の文字列（未設定の場合は "演出を選択..." と表示）
				if (cutsceneNames.empty())
				{
					ImGui::TextColored(ImVec4(1, 1, 0, 1), "カメラワークが登録されていません");
				}
				else
				{
					// 現在選択されている演出名をプレビュー表示用にする
					const char* previewValue = (strlen(currentData.eventCutsceneName) == 0) ? "演出を選択..." : currentData.eventCutsceneName;

					if (ImGui::BeginCombo("再生するカメラワーク", previewValue))
					{
						for (const auto& name : cutsceneNames)
						{
							bool isSelected = (name == currentData.eventCutsceneName);
							if (ImGui::Selectable(name.c_str(), isSelected))
							{
								// 選択された演出名をバッファにコピー
								strcpy_s(currentData.eventCutsceneName, sizeof(currentData.eventCutsceneName), name.c_str());
							}
							if (isSelected) ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
				}
			}
		}
	} 
	else if (currentData.category == EditCategory::Weapon)
	{
		ImGui::Combo("武器の種類", &currentData.subType, weaponCategoryNames, IM_ARRAYSIZE(weaponCategoryNames));

		// 位置
		ImGui::DragFloat3("生成位置", &currentData.position.x, 0.1f);

		// 耐久力
		ImGui::DragInt("耐久力", &currentData.durability, 1, 1, 10000);

		// 攻撃力
		ImGui::DragFloat("攻撃力", &currentData.attackPower, 0.1f, 0.0f, 10000.0f);

		// 壊れない武器かどうか
		ImGui::Checkbox("壊れるかどうか", &currentData.isUnbreakable);
	}

	ImGui::Separator();

	// 生成ボタン
	if (ImGui::Button("オブジェクトを生成"))
	{
		// 新しいオブジェクトを生成する前に、現在の配置リストの状態を履歴に保存する
		history_->SaveHistory(placementList);
		isDirty = true;

		// 新しい配置データを初期化
		PlacementData newData;
		newData.category = currentData.category;
		newData.subType = currentData.subType;
		newData.position = currentData.position;
		newData.rotate_ = currentData.rotate_;
		newData.scale = currentData.scale;
		newData.hp = currentData.hp;
		strcpy_s(newData.name, sizeof(newData.name), currentData.name);
		newData.eventType = currentData.eventType;
		strcpy_s(newData.eventStageDataFileName, sizeof(newData.eventStageDataFileName), currentData.eventStageDataFileName);
		strcpy_s(newData.eventCutsceneName, sizeof(newData.eventCutsceneName), currentData.eventCutsceneName);
		newData.behaviorTrees = currentData.behaviorTrees;
		newData.durability = currentData.durability;
		newData.attackPower = currentData.attackPower;
		newData.isUnbreakable = currentData.isUnbreakable;
		newData.standMotion = currentData.standMotion;
		newData.stanceMotion = currentData.stanceMotion;
		newData.walkMotion = currentData.walkMotion;
		newData.dashMotion = currentData.dashMotion;
		newData.avoidFrontMotion = currentData.avoidFrontMotion;
		newData.avoidBackMotion = currentData.avoidBackMotion;
		newData.avoidLeftMotion = currentData.avoidLeftMotion;
		newData.avoidRightMotion = currentData.avoidRightMotion;
		newData.guardMotion = currentData.guardMotion;

		// 実際のゲーム画面に生成してリストに追加
		spawner_->SpawnActualEntity(newData);
		placementList.push_back(newData);
		selectedIndex = static_cast<int>(placementList.size()) - 1;
	}

	ImGui::Separator();
}


/// @brief モーションの選択UIを表示する
/// @param motionType 
/// @param motionName 
bool StageEditorUIPlacement::MotionSelecter(const char* label, MotionType motionType, MotionConfig& motionConfig)
{
	// 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
	std::vector<std::string> motionNames = MotionManager::GetInstance()->GetMotionNames(motionType);

	bool isChanged = false; // モーションが変更されたかどうかを示すフラグ

	// モーション名のリストが空の場合はエラーメッセージを表示
	if (motionNames.empty())
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
	} else
	{
		// 現在選択されているモーション名をプレビュー用の文字列として設定
		const char* previewValue = motionConfig.name.empty() ? "モーションを選択..." : motionConfig.name.c_str();

		// モーション名選択用のコンボボックスを描画
		if (ImGui::BeginCombo(label, previewValue))
		{
			for (const auto& name : motionNames)
			{
				// 現在のモーション名と同じものが選択されている状態にする
				bool isSelected = (motionConfig.name == name);
				if (ImGui::Selectable(name.c_str(), isSelected))
				{
					motionConfig.name = name;
					motionConfig.handle = motionManager_->GetMotion(motionType, motionConfig.name);

					isChanged = true; // モーションが変更されたフラグを立てる
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}

	return isChanged;
}

/// @brief モーションの選択UIを表示する
/// @param motionType 
/// @param motionName 
/// @param placementList
void StageEditorUIPlacement::MotionSelecter(const char* label, MotionType motionType, MotionConfig& motionConfig, std::vector<PlacementData>& placementList, bool& isDirty)
{
	// 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
	std::vector<std::string> motionNames = MotionManager::GetInstance()->GetMotionNames(motionType);

	// モーション名のリストが空の場合はエラーメッセージを表示
	if (motionNames.empty())
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
	} 
	else
	{
		// 現在選択されているモーション名をプレビュー用の文字列として設定
		const char* previewValue = motionConfig.name.empty() ? "モーションを選択..." : motionConfig.name.c_str();

		// モーション名選択用のコンボボックスを描画
		if (ImGui::BeginCombo(label, previewValue))
		{
			for (const auto& name : motionNames)
			{
				// 現在のモーション名と同じものが選択されている状態にする
				bool isSelected = (motionConfig.name == name);
				if (ImGui::Selectable(name.c_str(), isSelected))
				{
					// モーションを変更する前に、現在の配置リストの状態を履歴に保存する
					history_->SaveHistory(placementList);
					isDirty = true;

					motionConfig.name = name;
					motionConfig.handle = motionManager_->GetMotion(motionType, motionConfig.name);
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
}