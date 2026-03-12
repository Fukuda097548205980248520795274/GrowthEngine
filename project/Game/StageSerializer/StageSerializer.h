#pragma once
#include "StageData/StageData.h"
#include <fstream>
#include <sstream>

class StageSerializer
{
public:

	/// @brief コンストラクタ
	StageSerializer() { directory_ = "./Assets/Stage/"; }

	/// @brief CSVで保存する
	/// @param stageData 
	void SaveCSV(const StageData& stageData);

	/// @brief CSVを読み込む
	/// @param stageData 
	/// @param fileName 
	void LoadCSV(StageData& stageData, const std::string& fileName);


private:

	/// @brief ディレクトリ
	std::string directory_{};
};

