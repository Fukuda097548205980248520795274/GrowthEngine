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
		currentData.instancePtr = nullptr;
		currentData.name[0] = '\0';
		currentData.templateName[0] = '\0';
		currentData.eventType = 0;
		currentData.isBattleAreaStart = false;
		currentData.isGameClear = false;
		currentData.eventStageDataFileName[0] = '\0';
		currentData.eventCutsceneName[0] = '\0';
		currentData.targetNavMeshGroupId = 0;
		currentData.targetNavMeshState = true;

		isInitialized = true;
	}

	// 配置モードの選択
	static int placementMode = 0;
	ImGui::RadioButton("テンプレートから配置", &placementMode, 0); ImGui::SameLine();
	ImGui::RadioButton("ステージオブジェクト直配置", &placementMode, 1);
	ImGui::Separator();

	if (placementMode == 0)
	{
		// プレハブ (テンプレート) の選択
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
						strcpy_s(currentData.templateName, sizeof(currentData.templateName), prefabNames[i].c_str());
						currentData.instancePtr = nullptr;
					}

					// 選択されたアイテムにフォーカスを設定
					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}
	}
	else
	{
		// --- テンプレートなしの直配置パラメータ ---
		currentData.templateName[0] = '\0'; // テンプレート名をクリア
		currentData.category = EditCategory::Object;

		if (ImGui::CollapsingHeader("ステージオブジェクト設定", ImGuiTreeNodeFlags_DefaultOpen))
		{
			// stageObjectTagNames[] ("None", "床", "壁", "イベントトリガー", "カメラガード") を利用
			ImGui::Combo("オブジェクト種別", &currentData.subType, stageObjectTagNames, IM_ARRAYSIZE(stageObjectTagNames));

			// イベントトリガー選択時のみ個別の基本パラメータを設定
			if (currentData.subType == static_cast<int>(StageObject::StageObjectTag::StaticEventTrigger))
			{
				ImGui::Combo("イベントタイプ", &currentData.eventType, eventTypeNames, IM_ARRAYSIZE(eventTypeNames));
				ImGui::Checkbox("戦闘エリア開始", &currentData.isBattleAreaStart);
				ImGui::Checkbox("ゲームクリア", &currentData.isGameClear);
			}
		}
	}

	ImGui::Separator();

	// 生成ボタン
	if (ImGui::Button("オブジェクトを生成"))
	{
		// 新しいオブジェクトを生成する前に、現在の配置リストの状態を履歴に保存する
		history_->SaveHistory(placementList);
		isDirty = true;

		// 新しい配置データを初期化
		PlacementData newData = currentData;
		newData.instancePtr = nullptr;

		// 実際のゲーム画面に生成してリストに追加
		if (spawner_->SpawnActualEntity(newData))
		{
			placementList.push_back(newData);
			selectedIndex = static_cast<int>(placementList.size()) - 1;
		}
	}

	ImGui::Separator();
}
