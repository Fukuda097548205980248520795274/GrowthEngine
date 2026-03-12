#pragma once
#include <cstdint>
#include <vector>
#include <string>

class StageData
{
public:

	// タイル
	enum class Tile
	{
		Air,
		Wall,
		Player,
	};


public:

	/// @brief リサイズ
	/// @param width 
	/// @param height 
	/// @param name 
	void Resize(int32_t width, int32_t height, const std::string& name);

	/// @brief 横幅
	int32_t width_ = 0;

	/// @brief 縦幅
	int32_t height_ = 0;

	/// @brief タイル
	std::vector<std::vector<Tile>> tiles_;

	/// @brief 名前
	std::string name_{};
};

