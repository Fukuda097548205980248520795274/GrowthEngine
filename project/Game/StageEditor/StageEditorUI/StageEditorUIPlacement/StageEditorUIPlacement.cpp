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

#include "../StageEditorUIHelper/StageEditorUIHelper.h"

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
	const std::vector<std::string>& behaviorTreeNames, const std::vector<std::string>& comboTreeNames, 
	const std::vector<std::string>& eventStageDataFileNames, const std::vector<std::string>& cutsceneNames)
{
	// オブジェクト配置モードのUIを描画
	ImGui::Text("--- オブジェクト配置 ---");

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


	// プレハブの選択UI
	if (ImGui::CollapsingHeader("プレハブ (テンプレート)", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 毎フレーム最新のプレハブ一覧を取得
		std::vector<std::string> prefabNames = StageEditorUIHelper::GetPrefabNames();

		// 選択中のプレハブ名プレビュー
		const char* previewPrefab = (selectedPrefabIdx_ >= 0 && selectedPrefabIdx_ < prefabNames.size())
			? prefabNames[selectedPrefabIdx_].c_str() : "テンプレートを選択...";

		if (ImGui::BeginCombo("プレハブ一覧", previewPrefab))
		{
			for (int i = 0; i < static_cast<int>(prefabNames.size()); ++i)
			{
				bool isSelected = (selectedPrefabIdx_ == i);
				if (ImGui::Selectable(prefabNames[i].c_str(), isSelected))
				{
					selectedPrefabIdx_ = i;

					// 選択されたプレハブを currentData に適用
					StageEditorUIHelper::LoadPrefab(prefabNames[i], currentData);
					currentData.instancePtr = nullptr;
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
	ImGui::Separator();



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
		ImGui::Separator();

		// キャラクターの基本設定UIを描画
		StageEditorUIHelper::DrawCharacterBaseSettings(currentData, placementList, isDirty, history_, false);

		// モーションの設定UIを描画
		StageEditorUIHelper::DrawCharacterMotionSettings(currentData, placementList, isDirty, history_, motionManager_, false);

		// プレイヤーと未選択以外　ビヘイビアツリーデータ
		if (currentData.subType != 0 && currentData.subType != 1)
		{
			// 共通ヘルパーからビヘイビアツリーUIを描画
			StageEditorUIHelper::DrawBehaviorTreeSettings(currentData.behaviorTrees, behaviorTreeNames, isDirty);
		}
		else if (currentData.subType == 1) 
		{
			// プレイヤーの場合はコンボツリーUIを描画
			StageEditorUIHelper::DrawComboTreeSettings(currentData.comboTrees, comboTreeNames, isDirty);
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

			// 共通ヘルパーからトリガー設定UIを呼び出し
			StageEditorUIHelper::DrawEventTriggerSettings(currentData, placementList, isDirty, history_, spawner_, scene_, eventStageDataFileNames, cutsceneNames);
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
