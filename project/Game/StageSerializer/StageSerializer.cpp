#include "StageSerializer.h"
#include <cassert>

/// @brief CSVで保存する
/// @param stageData 
void StageSerializer::SaveCSV(const StageData& stageData)
{
	// 書き込み
	std::ofstream ofs((directory_ + stageData.name_ + ".csv").c_str());

	for (int y = 0; y < stageData.height_; y++)
	{
		for (int x = 0; x < stageData.width_; x++)
		{
			ofs << static_cast<int32_t>(stageData.tiles_[y][x]);
			
			// 最後尾以外にカンマをつける
			if (x < stageData.width_ - 1)
				ofs << ",";
		}

		ofs << "\n";
	}
}

/// @brief CSVを読み込む
/// @param stageData 
/// @param fileName 
/// @return 
void StageSerializer::LoadCSV(StageData& stageData, const std::string& fileName)
{
	// 読み込み
	std::ifstream ifs((directory_ + fileName + ".csv").c_str());

	// ファイルが読めたかどうか
	if (!ifs.is_open())
		return;

	// 行
	std::string line;

	// 全体
	std::vector<std::vector<int>> rows;

	// ファイルの中身を変数に移す
	while (std::getline(ifs, line))
	{
		std::stringstream ss(line);
		std::string cell;
		std::vector<int> row;

		while ((std::getline(ss, cell, ',')))
		{
			row.push_back(std::atoi(cell.c_str()));
		}
		rows.push_back(row);
	}

	// ステージを作成し、サイズを決める
	stageData.Resize(static_cast<int32_t>(rows[0].size()), static_cast<int32_t>(rows.size()), fileName.c_str());

	// データに移す
	for (int y = 0; y < stageData.height_; ++y)
	{
		for (int x = 0; x < stageData.width_; ++x)
		{
			stageData.tiles_[y][x] = static_cast<StageData::Tile>(rows[y][x]);
		}
	}

	// ファイルを閉じる
	ifs.close();
}