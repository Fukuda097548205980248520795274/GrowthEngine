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