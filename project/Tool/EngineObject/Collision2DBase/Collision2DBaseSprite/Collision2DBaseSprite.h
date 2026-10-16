#pragma once
#include "../BaseCollision2DBase.h"

/// @brief 2D衝突スプライト判定インスタンス
class Collision2DInstanceSprite;

/// @brief 2D衝突スプライト判定
class Collision2DBaseSprite : public Engine::BaseCollision2DBase
{
public:

	/// @brief コンストラクタ
	/// @param name 
	Collision2DBaseSprite(const std::string& name);

	/// @brief インスタンスを作成する
	/// @return 
	Collision2DInstanceSprite* CreateInstance();
};