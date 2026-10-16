#pragma once
#include "../BaseCollision2DInstance.h"

/// @brief 2D衝突スプライト衝突インスタンス
class Collision2DInstanceSprite : public Engine::BaseCollision2DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision2DInstanceSprite(Engine::Collision2D::Sprite* param);

	/// @brief パラメータ
	std::unique_ptr<Engine::Collision2D::Sprite> param_ = nullptr;
};