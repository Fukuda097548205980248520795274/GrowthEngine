#pragma once
#include <string>
#include <vector>

struct StageInfo
{
	std::string fileName;
	std::string displayName;
};

class StageSelectEditor
{
public:

	/// @brief 初期化
	void Initialize();

	/// @brief 更新処理
	void DrawUI();

	/// @brief ステージリストを取得する
	/// @return 
	const std::vector<StageInfo>& GetStageList() const { return stageList_; }

	/// @brief チュートリアルステージのファイル名を取得する
	/// @return 
	const std::string& GetTutorialStageName() const { return tutorialStageName_; }

private:

	/// @brief フォルダからステージファイル名一覧を取得する
	void FetchAvailableFiles();

	/// @brief 設定をJSONに保存する
	void Save();

	/// @brief JSONから設定を読み込む
	void Load();

private:

	/// @brief 登録済みのステージリスト
	std::vector<StageInfo> stageList_;

	/// @brief ステージファイルの一覧
	std::vector<std::string> availableFiles_;

	/// @brief 選択中のステージファイルのインデックス
	int selectedFileIndex_ = 0;

	/// @brief 新規ステージの表示名
	char inputDisplayName_[256] = "";

	/// @brief チュートリアルステージのファイル名
	std::string tutorialStageName_ = "Tutorial";
};

