#pragma once
#include <string>
#include <vector>
#include "../StageData/StageData.h"
#include <json.hpp>

using json = nlohmann::json;

class StageSpawner;
class NavMesh;

class StageFileManager
{
public:

    /// @brief コンストラクタ
    /// @param directory 
	StageFileManager(const std::string& directory) : stageDataDir_(directory) {}

	/// @brief ファイルにステージデータを保存する
    /// @param filename 
    /// @param dataList 
    /// @return 
    bool SaveToFile(const std::string& filename, const std::vector<PlacementData>& dataList, const NavMesh* navMesh);

	/// @brief ファイルからステージデータを読み込む
    /// @param filename 
    /// @param outDataList 
    /// @param spawner 
    /// @return 
    bool LoadFromFile(const std::string& filename, std::vector<PlacementData>& outDataList, StageSpawner* spawner, NavMesh* navMesh);

	/// @brief 保存されているステージファイルのリストを取得する
    /// @return 
    std::vector<std::string> GetSavedStageFiles() const;

private:

	// ステージデータの保存先ディレクトリ
    std::string stageDataDir_;
};

