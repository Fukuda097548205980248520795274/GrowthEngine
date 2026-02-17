#pragma once
#include "Handle/Handle.h"
#include "Data/SpriteData/SpriteData.h"
#include <memory>
#include <string>

class GrowthEngine;

class Sprite
{
public:

	/// @brief コンストラクタ
	/// @param hTexture 
	/// @param name 
	Sprite(TextureHandle hTexture, const std::string& name);

	/// @brief 描画処理
	void Draw();

	/// @brief パラメータ
	Engine::Sprite::Param* param_ = nullptr;


private:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;

	// ハンドル
	SpriteHandle hSprite_ = 0;

	// 名前
	std::string name_{};
};

