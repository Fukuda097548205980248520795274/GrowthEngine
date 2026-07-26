#pragma once
#include "GrowthEngine.h"

struct UIElementData
{
	// UI要素の名前（ヒエラルキー表示用）
	std::string name;

	// 保存・復元用のテクスチャファイル名
	std::string textureFilename;

	// スプライト本体
	std::unique_ptr<Sprite> sprite;
};