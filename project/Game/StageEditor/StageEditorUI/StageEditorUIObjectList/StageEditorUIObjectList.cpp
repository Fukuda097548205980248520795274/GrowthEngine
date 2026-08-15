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
	bool& hasCopiedData, PlacementData& copiedData, NavMesh* navMesh,
	const std::vector<std::string> behaviorTreeNames, const std::vector<std::string>& comboTreeNames, 
	const std::vector<std::string> eventStageDataFileNames, const std::vector<std::string>& cutsceneNames)
{
	if (!ImGui::Begin("Object List"))
	{
		ImGui::End();
		return;
	}

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 8.0f));


	ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "リストフィルター");

	// テキスト検索フィルター
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

	// リスト表示領域の開始
	bool isVisible = ImGui::BeginChild("ObjectListRegion", ImVec2(0, 150), true);

	// オブジェクトのリスト表示
	if (isVisible)
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

			// 選択状態の判定
			bool isSelected = (multiSelectedIndices_.count(i) > 0);

			// 選択されたら selectedIndex_ を更新
			if (ImGui::Selectable(label.c_str(), selectedIndex == i))
			{
				if (ImGui::GetIO().KeyCtrl)
				{
					// Ctrlキーが押されている場合：選択のトグル反転
					if (isSelected) multiSelectedIndices_.erase(i);
					else multiSelectedIndices_.insert(i);
				}
				else if (ImGui::GetIO().KeyShift && !multiSelectedIndices_.empty())
				{
					// Shiftキーが押されている場合：範囲選択
					int minIdx = std::min(selectedIndex, i);
					int maxIdx = std::max(selectedIndex, i);
					multiSelectedIndices_.clear();
					for (int j = minIdx; j <= maxIdx; ++j)
					{
						multiSelectedIndices_.insert(j);
					}
				}
				else
				{
					// 通常クリック：単一選択
					multiSelectedIndices_.clear();
					multiSelectedIndices_.insert(i);
				}

				// 互換性のため、代表インデックスを記録
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

					// 複製したオブジェクトの名前を一意にする
					std::string uniqueName = GenerateUniqueName(newData.name, -1, placementList);
					strncpy_s(newData.name, uniqueName.c_str(), sizeof(newData.name) - 1);

					// 完全に重ならないように位置を少しずらす
					newData.position.x += 0.5f;
					newData.position.z += 0.5f;
					newData.instancePtr = nullptr; // 新しい実体を作るため初期化

					// 実体を生成して追加
					PlacementData weaponData = {};
					spawner_->SpawnActualEntity(newData, weaponData);
					placementList.push_back(newData);
					if (weaponData.instancePtr)
					{
						placementList.push_back(weaponData);
					}

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

					// 複数選択状態もリセットする
					multiSelectedIndices_.clear();
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

					// 貼り付けたオブジェクトの名前を一意にする
					std::string uniqueName = GenerateUniqueName(newData.name, -1, placementList);
					strncpy_s(newData.name, uniqueName.c_str(), sizeof(newData.name) - 1);

					PlacementData weaponData = {};
					spawner_->SpawnActualEntity(newData, weaponData);
					placementList.push_back(newData);
					if (weaponData.instancePtr)
					{
						placementList.push_back(weaponData);
					}
					selectedIndex = static_cast<int>(placementList.size()) - 1;
					listChanged = true;
				}
				ImGui::EndPopup();
			}
		}
	}

	ImGui::EndChild();


	// リストの変更があった場合、選択中のインデックスを安全に更新する
	for (auto it = multiSelectedIndices_.begin(); it != multiSelectedIndices_.end(); )
	{
		if (*it >= placementList.size()) it = multiSelectedIndices_.erase(it);
		else ++it;
	}

	// もし選択中のインデックスがリストの範囲外になった場合、選択状態をリセットする
	if (multiSelectedIndices_.empty())
		selectedIndex = -1;


	ImGui::Separator();


	// 選択中のオブジェクトの詳細編集UIを描画する
	if (multiSelectedIndices_.size() == 1 && selectedIndex >= 0 && selectedIndex < placementList.size())
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



		// 入力が完了した（エンターを押すか別の場所をクリックした）タイミングで重複チェック
		ImGui::InputText("オブジェクト名", target.name, sizeof(target.name));
		if (ImGui::IsItemDeactivatedAfterEdit())
		{
			history_->SaveHistory(placementList); // 変更されるので履歴を保存
			isDirty = true;

			// 重複のない名前に自動変換して上書きする
			std::string uniqueName = GenerateUniqueName(target.name, selectedIndex, placementList);
			strncpy_s(target.name, uniqueName.c_str(), sizeof(target.name) - 1);
		}
		ImGui::Separator();



		// カテゴリごとの編集項目
		if (target.category == EditCategory::Character)
		{
			// キャラクターの場合、キャラクター固有のUIを描画する
			Character* charPtr = static_cast<Character*>(target.instancePtr);

			// 共通ヘルパーからキャラクターの基本設定UIを描画し、変更があったかどうかを取得
			StageEditorUIHelper::DrawCharacterBaseSettings(target, placementList, isDirty, history_, true);


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


			// モーション設定UIを描画し、変更があったかどうかを取得
			bool motionChanged = 
				StageEditorUIHelper::DrawCharacterMotionSettings(target, placementList, isDirty, history_, motionManager_, true);

			// もしモーションのどれかが変更されたら、実際のキャラクターオブジェクトにアニメーションハンドルを更新する
			if (isChangeAnimation_)
			{
				AnimationHandleData animationData;
				animationData.hStandMotion = motionManager_->GetMotion(MotionType::Stand, target.standMotion.name);
				animationData.hStanceMotion = motionManager_->GetMotion(MotionType::Stance, target.stanceMotion.name);
				animationData.hWalkMotion = motionManager_->GetMotion(MotionType::Walk, target.walkMotion.name);
				animationData.hDashMotion = motionManager_->GetMotion(MotionType::Dash, target.dashMotion.name);
				animationData.hAvoidFrontMotion = motionManager_->GetMotion(MotionType::Avoid, target.avoidFrontMotion.name);
				animationData.hAvoidBackMotion = motionManager_->GetMotion(MotionType::Avoid, target.avoidBackMotion.name);
				animationData.hAvoidLeftMotion = motionManager_->GetMotion(MotionType::Avoid, target.avoidLeftMotion.name);
				animationData.hAvoidRightMotion = motionManager_->GetMotion(MotionType::Avoid, target.avoidRightMotion.name);
				animationData.hGuardMotion = motionManager_->GetMotion(MotionType::Guard, target.guardMotion.name);

				charPtr->SetAnimationHandle(animationData);

				isChangeAnimation_ = false; // フラグをリセット
			}

			// プレイヤーと未選択以外　ビヘイビアツリーデータ
			if (target.subType != static_cast<int32_t>(CharacterTag::None) && target.subType != static_cast<int32_t>(CharacterTag::Player))
			{
				// 共通ヘルパーからビヘイビアツリーUIを描画
				StageEditorUIHelper::DrawBehaviorTreeSettings(target.behaviorTrees, behaviorTreeNames, isDirty);
			}
			else if (target.subType == static_cast<int32_t>(CharacterTag::Player))
			{
				// 共通ヘルパーからコンボツリーUIを描画
				StageEditorUIHelper::DrawComboTreeSettings(target.comboTrees, comboTreeNames, isDirty);
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
					StageEditorUIHelper::DrawEventTriggerSettings(target, placementList, isDirty, history_, spawner_, scene_, eventStageDataFileNames, cutsceneNames);

					// イベントトリガーの種類が「生成イベント」の場合、生成ファイル編集UIを表示する
					if (target.eventType == static_cast<int32_t>(StaticEventTrigger::EventType::ObjectSpawn))
					{
						std::string fileName = target.eventStageDataFileName;
						LoadPreviewData(fileName);

						// バトルエリア開始のチェックボックス
						ImGui::Checkbox("バトルエリア開始", &target.isBattleAreaStart);

						if (target.isBattleAreaStart)
						{
							// ゲームクリアのチェックボックス
							ImGui::Checkbox("ゲームクリアにつなげるか", &target.isGameClear);
						}

						// 生成ファイル編集のUI
						ImGui::Separator();
						if (ImGui::TreeNode(("生成ファイル編集: " + fileName + ".json").c_str()))
						{
							// 追加・保存ボタン
							if (ImGui::Button("オブジェクト追加"))
							{
								PlacementData newData;
								newData.position = target.position; // 初期位置をトリガーと同じ位置に
								cachedPreviewData_.push_back(newData);
								selectedPreviewIndex_ = static_cast<int32_t>(cachedPreviewData_.size()) - 1;
							}
							ImGui::SameLine();
							if (ImGui::Button("変更を保存 (上書き)"))
							{
								SavePreviewData();
							}

							// プレビューデータの一覧リスト
							if (ImGui::BeginListBox("生成オブジェクト一覧"))
							{
								for (int i = 0; i < cachedPreviewData_.size(); ++i)
								{
									std::string label = "Item " + std::to_string(i) + " (Cat:" + std::to_string(static_cast<int>(cachedPreviewData_[i].category)) + ")";
									if (ImGui::Selectable(label.c_str(), selectedPreviewIndex_ == i))
									{
										selectedPreviewIndex_ = i;
									}
								}
								ImGui::EndListBox();
							}

							// 選択されたプレビューデータの詳細編集
							if (selectedPreviewIndex_ >= 0 && selectedPreviewIndex_ < cachedPreviewData_.size())
							{
								auto& editTarget = cachedPreviewData_[selectedPreviewIndex_];
								ImGui::Text("--- 選択中の生成オブジェクト設定 ---");

								// オブジェクト名の編集
								ImGui::InputText("オブジェクト名", editTarget.name, sizeof(editTarget.name));

								int currentCategory = static_cast<int>(editTarget.category);
								// categoryNamesは4要素(HUD含む)ですが、配置可能な3要素のみ表示します
								if (ImGui::Combo("カテゴリ", &currentCategory, categoryNames, 3))
								{
									editTarget.category = static_cast<EditCategory>(currentCategory);
									editTarget.subType = 0; // カテゴリが変わったらタイプをリセットする
								}

								// カテゴリに応じたサブタイプのコンボボックスを表示
								if (editTarget.category == EditCategory::Character)
								{
									ImGui::Combo("タイプ", &editTarget.subType, characterTagNames, IM_ARRAYSIZE(characterTagNames));
								}
								else if (editTarget.category == EditCategory::Object)
								{
									ImGui::Combo("タイプ", &editTarget.subType, stageObjectTagNames, IM_ARRAYSIZE(stageObjectTagNames));
								}
								else if (editTarget.category == EditCategory::Weapon)
								{
									ImGui::Combo("タイプ", &editTarget.subType, weaponCategoryNames, IM_ARRAYSIZE(weaponCategoryNames));
								}

								ImGui::Separator();

								// 位置・回転・スケールの調整
								ImGui::DragFloat3("位置 (Position)", &editTarget.position.x, 0.1f);
								ImGui::DragFloat3("回転 (Rotation)", &editTarget.rotate_.x, 0.1f);
								ImGui::DragFloat3("スケール (Scale)", &editTarget.scale.x, 0.1f); // スケールも調整可能にしておくと便利です

								// 削除ボタン
								ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
								if (ImGui::Button("このオブジェクトを削除"))
								{
									cachedPreviewData_.erase(cachedPreviewData_.begin() + selectedPreviewIndex_);
									selectedPreviewIndex_ = -1;
								}
								ImGui::PopStyleColor();
							}
							ImGui::TreePop();
						}

						// プレビュー表示
						GrowthEngine* engine = GrowthEngine::GetInstance();
						for (int i = 0; i < cachedPreviewData_.size(); ++i)
						{
							const auto& previewData = cachedPreviewData_[i];

							// 選択中のプレビューオブジェクトは色を変える
							Vector4 color = (i == selectedPreviewIndex_) ? Vector4(1.0f, 0.0f, 0.0f, 1.0f) : Vector4(1.0f, 1.0f, 0.0f, 1.0f);
							Vector4 cubeColor = color;
							cubeColor.w = 0.5f; // 半透明

							engine->DrawDebugLine3D(target.position, previewData.position, color);
							engine->DrawDebugCube(previewData.position, previewData.rotate_, previewData.scale, cubeColor);
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
							history_->SaveHistory(placementList);
						}

						// 有効/無効の切り替え
						if (ImGui::Checkbox("切り替え後の状態 (チェックで有効)", &target.targetNavMeshState))
						{
							isDirty = true;
							history_->SaveHistory(placementList);
						}
					}
				}
			}
			else if (target.subType == static_cast<int>(StageObject::StageObjectTag::CameraGuard))
			{
				// カメラガードオブジェクトの場合、特定のUIを表示する
				CameraGuard* cameraGuardPtr = static_cast<CameraGuard*>(target.instancePtr);
				cameraGuardPtr->DrawDebugUI(&target, placementList, history_, &isDirty);
			}
		}
		else if (target.category == EditCategory::Weapon)
		{
			Weapon* weaponPtr = static_cast<Weapon*>(target.instancePtr);
			weaponPtr->DrawDebugUI(&target, placementList, history_, &isDirty);

			ImGui::Text("コンボツリー");
			StageEditorUIHelper::DrawComboTreeSettings(target.comboTrees, comboTreeNames, isDirty);

			ImGui::Text("ビヘイビアツリー");
			StageEditorUIHelper::DrawBehaviorTreeSettings(target.behaviorTrees, behaviorTreeNames, isDirty);
		}
	}
	else if (multiSelectedIndices_.size() > 1)
	{
		// 複数選択されている場合、共通の編集UIを表示する

		ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "--- %d 個のオブジェクトを一括編集中 ---", multiSelectedIndices_.size());
		ImGui::Separator();

		// 一括移動（オフセット）
		if (ImGui::CollapsingHeader("一括移動 (相対座標)", ImGuiTreeNodeFlags_DefaultOpen))
		{
			static float bulkOffset[3] = { 0.0f, 0.0f, 0.0f };
			ImGui::DragFloat3("移動量", bulkOffset, 0.1f);
			if (ImGui::Button("適用 (移動)"))
			{
				history_->SaveHistory(placementList); // 履歴保存
				for (int idx : multiSelectedIndices_)
				{
					placementList[idx].position.x += bulkOffset[0];
					placementList[idx].position.y += bulkOffset[1];
					placementList[idx].position.z += bulkOffset[2];
				}
				isDirty = true;
				bulkOffset[0] = bulkOffset[1] = bulkOffset[2] = 0.0f; // リセット
			}
		}

		// プレハブの一括適用
		if (ImGui::CollapsingHeader("プレハブ (テンプレート) の一括適用", ImGuiTreeNodeFlags_DefaultOpen))
		{
			ImGui::TextWrapped("選択中のすべてのオブジェクトに対して、座標を維持したままプレハブのパラメータを上書きします。");

			std::vector<std::string> prefabNames = StageEditorUIHelper::GetPrefabNames();
			static int bulkPrefabIdx = -1;
			const char* previewPrefab = (bulkPrefabIdx >= 0 && bulkPrefabIdx < prefabNames.size())
				? prefabNames[bulkPrefabIdx].c_str() : "テンプレートを選択...";

			if (ImGui::BeginCombo("適用するプレハブ", previewPrefab))
			{
				for (int i = 0; i < prefabNames.size(); ++i)
				{
					if (ImGui::Selectable(prefabNames[i].c_str(), bulkPrefabIdx == i)) bulkPrefabIdx = i;
				}
				ImGui::EndCombo();
			}

			if (ImGui::Button("選択中の全オブジェクトに適用") && bulkPrefabIdx >= 0)
			{
				history_->SaveHistory(placementList);
				for (int idx : multiSelectedIndices_)
				{
					// 座標と実体ポインタを退避
					auto originalPos = placementList[idx].position;
					auto originalRot = placementList[idx].rotate_;
					void* backupPtr = placementList[idx].instancePtr;

					// プレハブのデータをロード
					StageEditorUIHelper::LoadPrefab(prefabNames[bulkPrefabIdx], placementList[idx]);

					// 座標とポインタを復元（パラメータとカテゴリだけが書き換わる）
					placementList[idx].position = originalPos;
					placementList[idx].rotate_ = originalRot;
					placementList[idx].instancePtr = backupPtr;

					// もし実体が配置済みなら再生成（StageSpawnerの仕様に合わせて更新）
					if (backupPtr != nullptr && spawner_ != nullptr)
					{
						spawner_->DeleteActualEntity(placementList[idx]);
						PlacementData weaponData = {};
						spawner_->SpawnActualEntity(placementList[idx], weaponData);
					}
				}
				isDirty = true;
			}
		}

		// 一括削除
		ImGui::Separator();
		if (ImGui::Button("選択中のオブジェクトを一括削除", ImVec2(-1, 30)))
		{
			history_->SaveHistory(placementList);

			// 配列の要素削除によるインデックスのズレを防ぐため、後ろ(降順)から削除する
			for (auto it = multiSelectedIndices_.rbegin(); it != multiSelectedIndices_.rend(); ++it)
			{
				spawner_->DeleteActualEntity(placementList[*it]);
				placementList.erase(placementList.begin() + *it);
			}
			multiSelectedIndices_.clear();
			selectedIndex = -1;
			isDirty = true;
		}
	}

	ImGui::PopStyleVar();

	ImGui::End();
}


/// @brief プレビュー用の配置データを読み込む
/// @param fileName 
void StageEditorUIObjectList::LoadPreviewData(const std::string& fileName)
{
	// ファイル名が変わっていない場合は読み込みをスキップ
	if (currentPreviewFileName_ == fileName) return;

	cachedPreviewData_.clear();
	currentPreviewFileName_ = fileName;

	if (fileName.empty()) return;

	std::string filePath = "./Assets/Parameter/StageData/" + fileName + ".json";
	std::ifstream ifs(filePath);
	if (!ifs.is_open()) return;

	try
	{
		nlohmann::json j;
		ifs >> j;
		ifs.close();

		if (j.contains("objects") && j["objects"].is_array())
		{
			for (const auto& objectDataJson : j["objects"])
			{
				PlacementData data;
				fromJson(objectDataJson, data); // StageData.h の関数を利用
				cachedPreviewData_.push_back(data);
			}
		}
	}
	catch (const std::exception& e)
	{
		(void)e;
	}
}

/// @brief プレビュー用の配置データを保存する
void StageEditorUIObjectList::SavePreviewData()
{
	if (currentPreviewFileName_.empty()) return;

	std::string filePath = "./Assets/Parameter/StageData/" + currentPreviewFileName_ + ".json";

	nlohmann::json j;
	j["objects"] = nlohmann::json::array(); // 空の配列を作成

	for (const auto& data : cachedPreviewData_)
	{
		nlohmann::json objJson;
		toJson(objJson, data); // PlacementData を JSON に変換
		j["objects"].push_back(objJson);
	}

	std::ofstream ofs(filePath);
	if (ofs.is_open())
	{
		ofs << j.dump(4); // 4マスのインデントをつけて綺麗に保存
		ofs.close();
	}
}