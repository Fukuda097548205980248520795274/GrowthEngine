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

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(ImGui::GetStyle().ItemSpacing.x, 8.0f));

	ImGui::Text("配置されたオブジェクト :");


	// 項目の追加や削除があった場合に、走査中のリストが変更されてバグるのを防止するためのフラグ
	bool listChanged = false;

	// オブジェクトのリスト表示
	ImGui::BeginChild("ObjectListRegion", ImVec2(0, 150), true);
	for (int i = 0; i < placementList.size(); ++i)
	{
		auto& data = placementList[i];

		// 表示用のラベルを作成 
		std::string label = "ID:" + std::to_string(i) + " ";
		if (data.name[0] == '\0')
		{
			if (data.category == EditCategory::Character) label += characterTagNames[data.subType];
			else if (data.category == EditCategory::Object) label += stageObjectTagNames[data.subType];
			else if (data.category == EditCategory::Weapon) label += weaponCategoryNames[data.subType];
		} else
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


	ImGui::Separator();


	// 選択中のオブジェクトがある場合、編集UIを表示
	if (selectedIndex >= 0 && selectedIndex < placementList.size())
	{
		auto& target = placementList[selectedIndex];
		ImGui::Text("--- 編集中のオブジェクト ---");

		// オブジェクト名の編集
		ImGui::InputText("オブジェクト名", target.name, sizeof(target.name));

		ImGui::Separator();

		// カテゴリごとの編集項目
		if (target.category == EditCategory::Character)
		{
			// 選択されたオブジェクトの実体をキャラクター型として扱う
			Character* charPtr = static_cast<Character*>(target.instancePtr);
			charPtr->DrawDebugUI(&target, placementList, history_, &isDirty);

			// アニメーション変更フラグを初期化
			isChangeAnimation_ = false;

			// モーション選択UI
			MotionSelecter("待機モーション", MotionType::Stand, target.standMotion, placementList, isDirty);
			MotionSelecter("戦闘モーション", MotionType::Stance, target.stanceMotion, placementList, isDirty);
			MotionSelecter("歩行モーション", MotionType::Walk, target.walkMotion, placementList, isDirty);
			MotionSelecter("ダッシュモーション", MotionType::Dash, target.dashMotion, placementList, isDirty);
			MotionSelecter("前方回避モーション", MotionType::Avoid, target.avoidFrontMotion, placementList, isDirty);
			MotionSelecter("後方回避モーション", MotionType::Avoid, target.avoidBackMotion, placementList, isDirty);
			MotionSelecter("左回避モーション", MotionType::Avoid, target.avoidLeftMotion, placementList, isDirty);
			MotionSelecter("右回避モーション", MotionType::Avoid, target.avoidRightMotion, placementList, isDirty);
			MotionSelecter("防御モーション", MotionType::Guard, target.guardMotion, placementList, isDirty);

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
				ImGui::Separator();
				if (ImGui::CollapsingHeader("ビヘイビアツリー設定")) 
				{
					if (DrawBehaviorTreeSelector("None (待機)", target.behaviorTrees.noneStateBT)) isDirty = true;
					if (DrawBehaviorTreeSelector("Dash（ダッシュ）", target.behaviorTrees.dashStateBT)) isDirty = true;

					if (DrawBehaviorTreeSelector("Grabbing（掴み）", target.behaviorTrees.grabbingStateBT)) isDirty = true;
					if (DrawBehaviorTreeSelector("Grabbed（掴まれ）", target.behaviorTrees.grabbedStateBT)) isDirty = true;
					if (DrawBehaviorTreeSelector("Guard（ガード）", target.behaviorTrees.guardStateBT)) isDirty = true;

					if (DrawBehaviorTreeSelector("LightDamage（弱ダメージ）", target.behaviorTrees.lightDamageStateBT)) isDirty = true;
					if (DrawBehaviorTreeSelector("HeavyDamage (強ダメージ)", target.behaviorTrees.heavyDamageStateBT)) isDirty = true;

					if (DrawBehaviorTreeSelector("LightDamage（倒れこみ）", target.behaviorTrees.downFallingStateBT)) isDirty = true;
					if (DrawBehaviorTreeSelector("DownLying（ダウン）", target.behaviorTrees.downLyingStateBT)) isDirty = true;
					if (DrawBehaviorTreeSelector("DownGettingUp（起き上がり）", target.behaviorTrees.downGettingUpStateBT)) isDirty = true;
					if (DrawBehaviorTreeSelector("DownStagger（ダウン怯み）", target.behaviorTrees.downStaggerStateBT)) isDirty = true;
					if (DrawBehaviorTreeSelector("BlownAway（吹き飛びあがり）", target.behaviorTrees.blownAwayStateBT)) isDirty = true;
					if (DrawBehaviorTreeSelector("BlownFalling（吹き飛び落下）", target.behaviorTrees.blownFallingStateBT)) isDirty = true;

					if (DrawBehaviorTreeSelector("Repel（弾き）", target.behaviorTrees.repelStateBT)) isDirty = true;
					if (DrawBehaviorTreeSelector("Deflect（受け流し）", target.behaviorTrees.deflectStateBT)) isDirty = true;

					if (DrawBehaviorTreeSelector("Repelled（弾かれ）", target.behaviorTrees.repelledStateBT)) isDirty = true;
					if (DrawBehaviorTreeSelector("Deflected（受け流され）", target.behaviorTrees.deflectedStateBT)) isDirty = true;

					if (DrawBehaviorTreeSelector("Avoid（回避）", target.behaviorTrees.avoidStateBT)) isDirty = true;
					if (DrawBehaviorTreeSelector("Dead（死亡）", target.behaviorTrees.deadStateBT)) isDirty = true;
				}
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

					ImGui::Unindent(); // 見栄え調整用（必要に応じて）
					ImGui::Separator();
					ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "--- イベントトリガー設定 ---");
					ImGui::Indent();

					int currentType = target.eventType;

					if (ImGui::Combo("イベントタイプ", &currentType, eventTypeNames, IM_ARRAYSIZE(eventTypeNames)))
					{
						// 変更前に履歴を保存
						history_->SaveHistory(placementList);
						isDirty = true;

						// データを更新
						target.eventType = currentType;
						eventTriggerPtr->SetEventType(currentType);

						// 配置されている実体を再生成して反映する
						spawner_->SpawnActualEntity(target);
					}

					
					if (target.eventType == 1) // 1: 敵生成(ObjectSpawn) の場合
					{
						ImGui::Text("ステージデータの設定");

						// プレビュー用の文字列（未設定の場合は "ステージデータを選択..." と表示）
						std::string currentSdName = target.eventStageDataFileName;
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
									strcpy_s(target.eventStageDataFileName, sizeof(target.eventStageDataFileName), name.c_str());
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
					else if (target.eventType == 2)
					{
						std::vector<std::string> cutsceneNames = scene_->GetCutsceneManager()->GetCutsceneNames();

						if (cutsceneNames.empty())
						{
							ImGui::TextColored(ImVec4(1, 1, 0, 1), "カメラワークが登録されていません");
						}
						else
						{
							const char* previewValue = (strlen(target.eventCutsceneName) == 0) ? "演出を選択..." : target.eventCutsceneName;

							if (ImGui::BeginCombo("再生するカメラワーク", previewValue))
							{
								for (const auto& name : cutsceneNames)
								{
									bool isSelected = (name == target.eventCutsceneName);
									if (ImGui::Selectable(name.c_str(), isSelected))
									{
										history_->SaveHistory(placementList); // 履歴を保存
										strcpy_s(target.eventCutsceneName, sizeof(target.eventCutsceneName), name.c_str());
										isDirty = true; // 変更フラグを立てる

										// すでに配置されている実体（トリガー）にも即座に反映する場合
										if (target.instancePtr)
										{
											auto* trigger = static_cast<StaticEventTrigger*>(target.instancePtr);
											trigger->SetEventStringParam(name.c_str());
										}
									}
									if (isSelected) ImGui::SetItemDefaultFocus();
								}
								ImGui::EndCombo();
							}
						}
					}
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


/// @brief モーションの選択UIを表示する
/// @param motionType 
/// @param motionName 
bool StageEditorUIObjectList::MotionSelecter(const char* label, MotionType motionType, MotionConfig& motionConfig)
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
void StageEditorUIObjectList::MotionSelecter(const char* label, MotionType motionType, MotionConfig& motionConfig, std::vector<PlacementData>& placementList, bool& isDirty)
{
	// 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
	std::vector<std::string> motionNames = MotionManager::GetInstance()->GetMotionNames(motionType);

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
