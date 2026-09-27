#pragma once
#include <string>
#include <vector>
#include "../StageData/StageData.h"
#include <json.hpp>

using json = nlohmann::json;

class StageSpawner;
class NavMesh;
class GameScene;

class StageFileManager
{
public:

	/// @brief コンストラクタ
	/// @param directory 
	/// @param scene 
	StageFileManager(const std::string& directory, GameScene* scene) : stageDataDir_(directory), scene_(scene) {}

	/// @brief ファイルにステージデータを保存する
	/// @param filename 
	/// @param dataList 
	/// @param navMesh 
	/// @param stageSettings 
	/// @return 
	bool SaveToFile(const std::string& filename, const std::vector<PlacementData>& dataList, const NavMesh* navMesh, const StageSettings& stageSettings);

	/// @brief ファイルからステージデータを読み込む
	/// @param filename 
	/// @param outDataList 
	/// @param spawner 
	/// @param navMesh 
	/// @param stageSettings 
	/// @return 
	bool LoadFromFile(const std::string& filename, std::vector<PlacementData>& outDataList, StageSpawner* spawner, NavMesh* navMesh, StageSettings& stageSettings);

	/// @brief ステージファイルをコピーする
	/// @param srcFileName 
	/// @param destFileName 
	/// @return 
	bool CopyStageFile(const std::string& srcFileName, const std::string& destFileName);

	/// @brief 保存されているステージファイルのリストを取得する
	/// @return 
	std::vector<std::string> GetSavedStageFiles() const;


private:

	// ステージデータの保存先ディレクトリ
	std::string stageDataDir_;

	/// @brief ゲームシーン
	GameScene* scene_ = nullptr;
};

