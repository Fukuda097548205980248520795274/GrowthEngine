#include "StageData.h"

/// @brief リサイズ
/// @param width 
/// @param height 
/// @param name 
void StageData::Resize(int32_t width, int32_t height, const std::string& name)
{
	// タイルをコピーする
	std::vector<std::vector<Tile>> prevTiles = tiles_;

	// 初期化する
	prevTiles.clear();

	// 値を取得する
	width_ = width;
	height_ = height;
	name_ = name;

	// サイズを確保する
	tiles_.resize(height);
	for (auto& tileWidth : tiles_)tileWidth.resize(width);

	if (prevTiles.empty())
		return;

	// 直前のデータを一部コピーする
	for (int i = 0; i < prevTiles.size(); ++i)
	{
		// 領域を超えてはいけない
		if (static_cast<int32_t>(tiles_.size()) >= i)
			break;

		for (int j = 0; j < prevTiles[i].size(); j++)
		{
			// 領域を超えてはいけない
			if (static_cast<int32_t>(tiles_[i].size()) >= j)
				break;

			// 値を渡す
			tiles_[i][j] = prevTiles[i][j];
		}
	}
}