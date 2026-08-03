#include "StageSelectEditor.h"
#include "GrowthEngine.h"

using json = nlohmann::json;

namespace
{
	/// @brief ステージデータのディレクトリ
	const std::string kStageDataDir = "./Assets/Parameter/StageData/";

	/// @brief ステージデータベースのJSONファイルのパス
	const std::string kDatabaseDir = "./Assets/Parameter/StageDatabase/";

	/// @brief ステージデータベースのJSONファイル名
	const std::string kDatabaseFilename = "StageDatabase.json";
}

/// @brief 初期化
void StageSelectEditor::Initialize()
{
	// 既存のリストを読み込む
	Load();

	// フォルダから選択可能なファイルのリストを取得する
	FetchAvailableFiles();
}

/// @brief 更新処理
void StageSelectEditor::DrawUI()
{
#ifdef DEVELOPMENT

	ImGui::Begin("Stage Select Editor");

	//　チュートリアルステージの設定UI
	ImGui::Text("Tutorial Stage Setting");
	if (!availableFiles_.empty())
	{
		if (ImGui::BeginCombo("Tutorial Stage", tutorialStageName_.c_str()))
		{
			for (const auto& file : availableFiles_)
			{
				const bool isSelected = (tutorialStageName_ == file);
				if (ImGui::Selectable(file.c_str(), isSelected))
				{
					tutorialStageName_ = file;
					Save(); // 選択変更時に即時保存
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "No stage files found.");
	}

	ImGui::Separator();

	// 新規ステージの登録UI
	ImGui::Text("Add New Stage");

	// 取得したファイル名のコンボボックス
	if (!availableFiles_.empty())
	{
		const char* previewValue = availableFiles_[selectedFileIndex_].c_str();
		if (ImGui::BeginCombo("File Name", previewValue))
		{
			for (int i = 0; i < availableFiles_.size(); i++)
			{
				const bool isSelected = (selectedFileIndex_ == i);
				if (ImGui::Selectable(availableFiles_[i].c_str(), isSelected))
				{
					selectedFileIndex_ = i;
				}
				if (isSelected) ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(1, 0, 0, 1), "No stage files found in directory.");
	}

	// 表示名の入力
	ImGui::InputText("Display Name", inputDisplayName_, IM_ARRAYSIZE(inputDisplayName_));

	// 登録ボタン
	if (ImGui::Button("Add to List") && !availableFiles_.empty())
	{
		StageInfo newStage;
		newStage.fileName = availableFiles_[selectedFileIndex_];

		// 入力がなければファイル名をそのまま表示名にする
		if (strlen(inputDisplayName_) > 0)
			newStage.displayName = inputDisplayName_;
		else
			newStage.displayName = newStage.fileName;

		stageList_.push_back(newStage);

		// 入力欄をクリア
		inputDisplayName_[0] = '\0';
		Save();
	}

	ImGui::Separator();

	// 登録済みステージのリスト表示
	ImGui::Text("Registered Stages");

	if (ImGui::BeginTable("StageListTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
	{
		ImGui::TableSetupColumn("File Name");
		ImGui::TableSetupColumn("Display Name");
		ImGui::TableSetupColumn("Action");
		ImGui::TableHeadersRow();

		for (size_t i = 0; i < stageList_.size(); ++i)
		{
			ImGui::TableNextRow();

			ImGui::TableNextColumn();
			ImGui::Text("%s", stageList_[i].fileName.c_str());

			ImGui::TableNextColumn();
			ImGui::Text("%s", stageList_[i].displayName.c_str());

			ImGui::TableNextColumn();
			ImGui::PushID(static_cast<int>(i)); // ID被り防止
			if (ImGui::Button("Delete"))
			{
				stageList_.erase(stageList_.begin() + i);
				Save();
				--i; // インデックスを調整
			}
			ImGui::PopID();
		}
		ImGui::EndTable();
	}

	ImGui::End();

#endif
}

/// @brief フォルダからステージファイル名一覧を取得する
void StageSelectEditor::FetchAvailableFiles()
{
	availableFiles_.clear();

	// ディレクトリが存在するか確認
	if (std::filesystem::exists(kStageDataDir))
	{
		for (const auto& entry : std::filesystem::directory_iterator(kStageDataDir))
		{
			if (entry.is_regular_file() && entry.path().extension() == ".json")
			{
				// ファイル名の拡張子を除いた部分を取得してリストに追加
				availableFiles_.push_back(entry.path().stem().string());
			}
		}
	}
}

/// @brief 設定をJSONに保存する
void StageSelectEditor::Save()
{
	// ディレクトリが存在しない場合は作成する
	if(!std::filesystem::exists(kDatabaseDir))
		std::filesystem::create_directories(kDatabaseDir);

	json j;
	j["tutorialStageName"] = tutorialStageName_;
	for (const auto& stage : stageList_)
	{
		json item;
		item["fileName"] = stage.fileName;
		item["displayName"] = stage.displayName;
		j["stages"].push_back(item);
	}

	std::ofstream ofs(kDatabaseDir + kDatabaseFilename);
	if (ofs.is_open())
	{
		ofs << j.dump(4); // 4スペースインデントで保存
	}
}

/// @brief JSONから設定を読み込む
void StageSelectEditor::Load()
{
	stageList_.clear();
	std::ifstream ifs(kDatabaseDir + kDatabaseFilename);
	if (!ifs.is_open()) return;

	try
	{
		json j;
		ifs >> j;

		// チュートリアルステージの設定を読み込む
		tutorialStageName_ = j.value("tutorialStageName", "Tutorial");

		if (j.contains("stages") && j["stages"].is_array())
		{
			for (const auto& item : j["stages"])
			{
				StageInfo info;
				info.fileName = item.value("fileName", "");
				info.displayName = item.value("displayName", "");
				stageList_.push_back(info);
			}
		}
	}
	catch (const std::exception& e)
	{
		// 読み込みエラー
		(void)e;
	}
}