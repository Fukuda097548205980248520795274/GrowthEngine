#include "StageEditorUIObjectList.h"
#include "StageEditor/StageSpawner/StageSpawner.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"
#include "BehaviorTree/BehaviorTreeEditor/BehaviorTreeEditor.h"
#include <numbers>

#include "Entity/Character/NPC/NPC.h"
#include "Entity/Character/Player/Player.h"
#include "Entity/Weapon/Weapon.h"

#include "StageObject/Floor/Floor.h"
#include "StageObject/Wall/Wall.h"
#include "StageObject/StaticEventTrigger/StaticEventTrigger.h"

#include "Scene/GameScene/GameScene.h"

#include "../StageEditorUIHelper/StageEditorUIHelper.h"

/// @brief コンストラクタ
/// @param spawner 
/// @param history 
StageEditorUIObjectList::StageEditorUIObjectList(StageSpawner* spawner, StageEditorHistory* history, GameScene* scene, BehaviorTreeEditor* behaviorTreeEditor)
	: spawner_(spawner), history_(history),scene_(scene), behaviorTreeEditor_(behaviorTreeEditor)
{
	// モーションマネージャのインスタンスを取得
	motionManager_ = MotionManager::GetInstance();
}

/// @brief 配置されたオブジェクトのリストを描画する
/// @param placementList 
/// @param selectedIndex 
/// @param isDirty 
/// @param navMesh 
void StageEditorUIObjectList::DrawWindow(std::vector<PlacementData>& placementList, int& selectedIndex, bool& isDirty,
	bool& hasCopiedData, PlacementData& copiedData, NavMesh* navMesh, const std::vector<std::string> behaviorTreeNames, const std::vector<std::string> eventStageDataFileNames)
{
	if (!ImGui::Begin("Object List"))
	{
		ImGui::End();
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 8.0f));


	ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "リストフィルター");

	// テキスト検索バーの描画 (入力された文字が objectFilter_ に格納されます)
	objectFilter_.Draw("検索 (##ObjectSearchFilter)", 180.0f);
	ImGui::SameLine();

	// カテゴリ絞り込みコンボボックス
	const char* filterCategoryNames[] = { "すべて", "キャラクター", "オブジェクト", "武器" };
	ImGui::SetNextItemWidth(120.0f);
	ImGui::Combo("カテゴリ##FilterCat", &categoryFilterIdx_, filterCategoryNames, IM_ARRAYSIZE(filterCategoryNames));

	// フィルターが有効な場合だけ「クリア」ボタンを表示する
	if (objectFilter_.IsActive() || categoryFilterIdx_ != 0)
	{
		ImGui::SameLine();
		if (ImGui::Button("クリア"))
		{
			objectFilter_.Clear();
			categoryFilterIdx_ = 0;
		}
	}


	ImGui::Separator();
	ImGui::Text("配置されたオブジェクト :");

	// 項目の追加や削除があった場合に、走査中のリストが変更されてバグるのを防止するためのフラグ
	bool listChanged = false;

	// オブジェクトのリスト表示
	if (ImGui::BeginChild("ObjectListRegion", ImVec2(0, 150), true))
	{
		for (int i = 0; i < placementList.size(); ++i)
		{
			auto& data = placementList[i];

			// カテゴリフィルターに合致しない場合は描画をスキップ
			if (categoryFilterIdx_ != 0)if (static_cast<int>(data.category) != (categoryFilterIdx_ - 1))
				continue;

			// テキスト検索フィルターに合致しない場合は描画をスキップ
			if (!objectFilter_.PassFilter(data.name))
				continue;

			// 表示用のラベルを作成 
			std::string label = "ID:" + std::to_string(i) + " ";
			if (data.name[0] == '\0')
			{
				if (data.category == EditCategory::Character) label += characterTagNames[data.subType];
				else if (data.category == EditCategory::Object) label += stageObjectTagNames[data.subType];
				else if (data.category == EditCategory::Weapon) label += weaponCategoryNames[data.subType];
			}
			else
			{
				label += data.name; // 名前が設定されていればそちらを優先して表示
			}

			// 各アイテムごとに一意のID空間を作る（右クリックメニューのバッティング防止）
			ImGui::PushID(i);

			// 選択されたら selectedIndex_ を更新
			if (ImGui::Selectable(label.c_str(), selectedIndex == i))
			{
				selectedIndex = i;
			}

			// 各項目に対する右クリックコンテキストメニュー
			if (ImGui::BeginPopupContextItem("ObjectItemContextMenu"))
			{
				selectedIndex = i; // 右クリックしたアイテムを自動的に選択状態にする

				// オブジェクトのコピー 
				if (ImGui::MenuItem("コピー"))
				{
					copiedData = placementList[i];

					// 新しいファイルで生成し直すため、実体へのポインタはリセットする
					copiedData.instancePtr = nullptr;
					hasCopiedData = true;
				}

				// オブジェクトの複製
				if (ImGui::MenuItem("複製"))
				{
					history_->SaveHistory(placementList);
					isDirty = true;

					// 選択中のデータを複製
					PlacementData newData = placementList[i];

					// 完全に重ならないように位置を少しずらす
					newData.position.x += 0.5f;
					newData.position.z += 0.5f;
					newData.instancePtr = nullptr; // 新しい実体を作るため初期化

					// 実体を生成して追加
					spawner_->SpawnActualEntity(newData);
					placementList.push_back(newData);

					// 複製したオブジェクトを選択状態にする
					selectedIndex = static_cast<int>(placementList.size()) - 1;
					listChanged = true;
				}

				ImGui::Separator();

				// オブジェクトの消去 (削除)
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
				if (ImGui::MenuItem("削除"))
				{
					history_->SaveHistory(placementList);
					isDirty = true;

					// ゲーム内実体の削除
					spawner_->DeleteActualEntity(placementList[i]);
					// リストから削除
					placementList.erase(placementList.begin() + i);

					selectedIndex = -1; // 選択状態をリセット
					listChanged = true;
				}
				ImGui::PopStyleColor();

				ImGui::EndPopup();
			}

			ImGui::PopID();

			// 項目が追加または削除されたら、安全のためにこのフレームの走査を終了する
			if (listChanged)
			{
				break;
			}
		}

		// アイテムの上以外で右クリックされたら、全体のコンテキストメニューを開く
		if (hasCopiedData)
		{
			// 右クリックされた位置にアイテムがない場合は、全体のコンテキストメニューを開く
			if (ImGui::BeginPopupContextWindow("ObjectListPasteRegionMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems))
			{
				if (ImGui::MenuItem("貼り付け"))
				{
					history_->SaveHistory(placementList);
					isDirty = true;

					// コピーしたデータを新しいオブジェクトとして生成
					PlacementData newData = copiedData;
					spawner_->SpawnActualEntity(newData);
					placementList.push_back(newData);
					selectedIndex = static_cast<int>(placementList.size()) - 1;
					listChanged = true;
				}
				ImGui::EndPopup();
			}
		}

		ImGui::EndChild();
	}


	ImGui::Separator();


	// 選択中のオブジェクトがある場合、編集UIを表示
	if (selectedIndex >= 0 && selectedIndex < placementList.size())
	{
		auto& target = placementList[selectedIndex];
		ImGui::Text("--- 編集中のオブジェクト ---");
		ImGui::Separator();

		// プレハブとして保存するためのUI
		static char newPrefabName[64] = "";
		ImGui::InputText("プレハブ保存名", newPrefabName, sizeof(newPrefabName));
		if (ImGui::Button("プレハブとして保存"))
		{
			if (strlen(newPrefabName) > 0)
			{
				// StageEditorUIHelper を使って保存
				StageEditorUIHelper::SavePrefab(newPrefabName, target);

				// 保存完了したら入力欄をリセット
				memset(newPrefabName, 0, sizeof(newPrefabName));
			}
		}
		ImGui::Separator();


		// オブジェクト名の編集
		ImGui::InputText("オブジェクト名", target.name, sizeof(target.name));
		ImGui::Separator();

		// カテゴリごとの編集項目
		if (target.category == EditCategory::Character)
		{
			// キャラクターの場合、キャラクター固有のUIを描画する
			Character* charPtr = static_cast<Character*>(target.instancePtr);
			charPtr->DrawDebugUI(&target, placementList, history_, &isDirty);

			// 共通ヘルパーからキャラクターの基本設定UIを描画し、変更があったかどうかを取得
			StageEditorUIHelper::DrawCharacterBaseSettings(target, placementList, isDirty, history_, true);

			// モーション設定UIを描画し、変更があったかどうかを取得
			bool motionChanged = 
				StageEditorUIHelper::DrawCharacterMotionSettings(target, placementList, isDirty, history_, motionManager_, true);

			// もしモーションのどれかが変更されたら、実際のキャラクターオブジェクトにアニメーションハンドルを更新する
			if (isChangeAnimation_)
			{
				AnimationHandleData animData;
				animData.hStandMotion = motionManager_->GetMotion(MotionType::Stand, target.standMotion.name);
				animData.hStanceMotion = motionManager_->GetMotion(MotionType::Stance, target.stanceMotion.name);
				animData.hWalkMotion = motionManager_->GetMotion(MotionType::Walk, target.walkMotion.name);
				animData.hDashMotion = motionManager_->GetMotion(MotionType::Dash, target.dashMotion.name);
				animData.hAvoidFrontMotion = motionManager_->GetMotion(MotionType::Avoid, target.avoidFrontMotion.name);
				animData.hAvoidBackMotion = motionManager_->GetMotion(MotionType::Avoid, target.avoidBackMotion.name);
				animData.hAvoidLeftMotion = motionManager_->GetMotion(MotionType::Avoid, target.avoidLeftMotion.name);
				animData.hAvoidRightMotion = motionManager_->GetMotion(MotionType::Avoid, target.avoidRightMotion.name);
				animData.hGuardMotion = motionManager_->GetMotion(MotionType::Guard, target.guardMotion.name);

				charPtr->SetAnimationHandle(animData);

				isChangeAnimation_ = false; // フラグをリセット
			}

			// プレイヤーと未選択以外　ビヘイビアツリーデータ
			if (target.subType != 0 && target.subType != 1)
			{
				// 共通ヘルパーからビヘイビアツリーUIを描画
				StageEditorUIHelper::DrawBehaviorTreeSettings(target.behaviorTrees, behaviorTreeNames, isDirty);
			}
		} 
		else if (target.category == EditCategory::Object)
		{
			if (target.subType == static_cast<int>(StageObject::StageObjectTag::Floor))
			{
				// フロアオブジェクトの場合、特定のUIを表示する
				Floor* floorPtr = static_cast<Floor*>(target.instancePtr);
				floorPtr->DrawDebugUI(&target, placementList, history_, &isDirty);
			} 
			else if (target.subType == static_cast<int>(StageObject::StageObjectTag::Wall))
			{
				// 壁オブジェクトの場合、特定のUIを表示する
				Wall* wallPtr = static_cast<Wall*>(target.instancePtr);
				wallPtr->DrawDebugUI(&target, placementList, history_, &isDirty);
			} 
			else if (target.subType == static_cast<int>(StageObject::StageObjectTag::StaticEventTrigger))
			{
				// イベントトリガーオブジェクトの場合、特定のUIを表示する
				StaticEventTrigger* eventTriggerPtr = static_cast<StaticEventTrigger*>(target.instancePtr);

				// イベントトリガーオブジェクトの場合、特定のUIを表示する
				if (eventTriggerPtr != nullptr)
				{
					eventTriggerPtr->DrawDebugUI(&target, placementList, history_, &isDirty);

					// 共通ヘルパーからイベントトリガー設定UIを描画
					StageEditorUIHelper::DrawEventTriggerSettings(target, placementList, isDirty, history_, spawner_, scene_, eventStageDataFileNames);
				}
			}
		}
		else if (target.category == EditCategory::Weapon)
		{
			Weapon* weaponPtr = static_cast<Weapon*>(target.instancePtr);
			weaponPtr->DrawDebugUI(&target, placementList, history_, &isDirty);
		}
	}

	ImGui::PopStyleVar();

	ImGui::End();
}
