#include "StageEditorUITemplate.h"
#include "StageEditor/StageSpawner/StageSpawner.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"
#include "Scene/GameScene/GameScene.h"
#include "../StageEditorUIHelper/StageEditorUIHelper.h"

/// @brief コンストラクタ
/// @param spawner 
/// @param history 
/// @param scene 
StageEditorUITemplate::StageEditorUITemplate(StageSpawner* spawner, StageEditorHistory* history, GameScene* scene)
	: spawner_(spawner), history_(history), scene_(scene)
{
	// モーションマネージャのインスタンスを取得
	motionManager_ = MotionManager::GetInstance();
}

/// @brief UIを描画する
/// @param isDirty 
/// @param behaviorTreeNames 
/// @param comboTreeNames 
/// @param eventStageDataFileNames 
/// @param cutsceneNames 
void StageEditorUITemplate::DrawUI(std::vector<PlacementData>& placementList, bool& isDirty,
	const std::vector<std::string>& behaviorTreeNames, const std::vector<std::string>& comboTreeNames,
	const std::vector<std::string>& eventStageDataFileNames, const std::vector<std::string>& cutsceneNames)
{
	if (!ImGui::Begin("Template"))
	{
		ImGui::End();
		return;
	}

	ImGui::Text("--- テンプレート (プレハブ) 編集 ---");

	// プレハブ一覧を取得
	std::vector<std::string> prefabNames = StageEditorUIHelper::GetPrefabNames();
	const char* previewPrefab = (selectedTemplateIdx_ >= 0 && selectedTemplateIdx_ < prefabNames.size())
		? prefabNames[selectedTemplateIdx_].c_str() : "テンプレートを選択...";

	// テンプレートの選択
	if (ImGui::BeginCombo("プレハブ一覧", previewPrefab))
	{
		for (int i = 0; i < static_cast<int>(prefabNames.size()); ++i)
		{
			bool isSelected = (selectedTemplateIdx_ == i);
			if (ImGui::Selectable(prefabNames[i].c_str(), isSelected))
			{
				selectedTemplateIdx_ = i;

				// 選択したテンプレートをロード
				StageEditorUIHelper::LoadPrefab(prefabNames[i], currentData_);
				strncpy_s(templateNameBuffer_, prefabNames[i].c_str(), sizeof(templateNameBuffer_) - 1);
			}

			if (isSelected) 
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::Separator();

	// 新規作成・保存UI
	ImGui::InputText("テンプレート名", templateNameBuffer_, sizeof(templateNameBuffer_));

	// ボタンによる保存要求
	bool isSaveRequested = ImGui::Button("保存 / 上書き");

	// 操作が分かりやすいようにツールチップを追加
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("ショートカット: Ctrl + S");
	}

	// ウィンドウにフォーカスがある時のみ、Ctrl + S での保存を受け付ける
	ImGuiIO& io = ImGui::GetIO();
	if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) && io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
	{
		isSaveRequested = true;
	}

	// 保存処理の実行
	if (isSaveRequested)
	{
		if (strlen(templateNameBuffer_) > 0)
		{
			// ヘルパー関数を利用して保存
			StageEditorUIHelper::SavePrefab(templateNameBuffer_, currentData_);
			isDirty = true;
		}
	}

	ImGui::Separator();

	// テンプレートが選択されている、または新規作成用にデータがある場合
	if (selectedTemplateIdx_ >= 0 || strlen(templateNameBuffer_) > 0)
	{
		ImGui::Text("詳細設定");

		// 大分類の選択
		int intCat = static_cast<int>(currentData_.category);
		if (ImGui::Combo("大分類", &intCat, categoryNames, IM_ARRAYSIZE(categoryNames)))
		{
			currentData_.category = static_cast<EditCategory>(intCat);
			currentData_.subType = 0;
		}

		if (currentData_.category == EditCategory::Character)
		{
			ImGui::Combo("キャラクター", &currentData_.subType, characterTagNames, IM_ARRAYSIZE(characterTagNames));

			// キャラクターの基本設定UIを描画し、変更があったかどうかを取得
			StageEditorUIHelper::DrawCharacterTemplateSettings(currentData_, isDirty);

			// プレイヤーと未選択以外　ビヘイビアツリーデータ
			if (currentData_.subType != static_cast<int32_t>(CharacterTag::None) && currentData_.subType != static_cast<int32_t>(CharacterTag::Player))
			{
				// 共通ヘルパーからビヘイビアツリーUIを描画
				StageEditorUIHelper::DrawBehaviorTreeSettings(currentData_.behaviorTrees, behaviorTreeNames, isDirty);
			}
			else if (currentData_.subType == static_cast<int32_t>(CharacterTag::Player))
			{
				// 共通ヘルパーからコンボツリーUIを描画
				StageEditorUIHelper::DrawComboTreeSettings(currentData_.comboTrees, comboTreeNames, isDirty);
			}
		}
		else if (currentData_.category == EditCategory::Object)
		{
			ImGui::Combo("オブジェクト", &currentData_.subType, stageObjectTagNames, IM_ARRAYSIZE(stageObjectTagNames));

			if (currentData_.subType == static_cast<int>(StageObject::StageObjectTag::StaticEventTrigger))
			{
				// 共通ヘルパーからイベントトリガー設定UIを描画
				StageEditorUIHelper::DrawEventTriggerSettings(currentData_, isDirty, scene_, eventStageDataFileNames, cutsceneNames);

				// イベントトリガーの種類が「生成イベント」の場合、生成ファイル編集UIを表示する
				if (currentData_.eventType == static_cast<int32_t>(StaticEventTrigger::EventType::ObjectSpawn))
				{
					std::string fileName = currentData_.eventStageDataFileName;
					LoadPreviewData(fileName);

					// バトルエリア開始のチェックボックス
					ImGui::Checkbox("バトルエリア開始", &currentData_.isBattleAreaStart);

					if (currentData_.isBattleAreaStart)
					{
						// ゲームクリアのチェックボックス
						ImGui::Checkbox("ゲームクリアにつなげるか", &currentData_.isGameClear);
					}

					// 生成ファイル編集のUI
					ImGui::Separator();
					if (ImGui::TreeNode(("生成ファイル編集: " + fileName + ".json").c_str()))
					{
						// 追加・保存ボタン
						if (ImGui::Button("オブジェクト追加"))
						{
							PlacementData newData;
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
							ImGui::InputText("オブジェクト名", editTarget.templateName, sizeof(editTarget.templateName));

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

						//engine->DrawDebugLine3D(currentData_.position, previewData.position, color);
						//engine->DrawDebugCube(previewData.position, previewData.rotate_, previewData.scale, cubeColor);
					}
				}
				else if (currentData_.eventType == static_cast<int32_t>(StaticEventTrigger::EventType::NavMeshStateChange))
				{
					ImGui::Separator();
					ImGui::Text("--- ナビメッシュ切り替え設定 ---");

					// グループIDの入力
					if (ImGui::InputInt("対象グループID", &currentData_.targetNavMeshGroupId))
					{
						isDirty = true;
						history_->SaveHistory(placementList);
					}

					// 有効/無効の切り替え
					if (ImGui::Checkbox("切り替え後の状態 (チェックで有効)", &currentData_.targetNavMeshState))
					{
						isDirty = true;
						history_->SaveHistory(placementList);
					}
				}
			}
		}
		else if (currentData_.category == EditCategory::Weapon)
		{
			ImGui::Combo("武器", &currentData_.subType, weaponCategoryNames, IM_ARRAYSIZE(weaponCategoryNames));

			// 共通ヘルパーから武器の基本設定UIを描画
			StageEditorUIHelper::DrawWeaponTemplateSettings(currentData_, isDirty);

			// 共通ヘルパーからコンボツリーUIを描画
			StageEditorUIHelper::DrawComboTreeSettings(currentData_.comboTrees, comboTreeNames, isDirty);

			// 共通ヘルパーからビヘイビアツリーUIを描画
			StageEditorUIHelper::DrawBehaviorTreeSettings(currentData_.behaviorTrees, behaviorTreeNames, isDirty);
		}
	}

	ImGui::End();
}

/// @brief プレビュー用データの読み込み
/// @param fileName 
void StageEditorUITemplate::LoadPreviewData(const std::string& fileName)
{
	// 同じファイル名なら再ロードしない
	if (fileName == currentLoadedFileName_) return;

	cachedPreviewData_.clear();
	selectedPreviewIndex_ = -1;
	currentLoadedFileName_ = fileName;

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

/// @brief プレビュー用データの保存
void StageEditorUITemplate::SavePreviewData()
{
	if (currentLoadedFileName_.empty()) return;

	std::string filePath = "./Assets/Parameter/StageData/" + currentLoadedFileName_ + ".json";

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