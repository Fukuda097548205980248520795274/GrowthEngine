#pragma once
#include "../BaseCollision2DInstance.h"

class Collision2DInstanceSprite : public Engine::BaseCollision2DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision2DInstanceSprite(Engine::Collision2D::Sprite* param) : param_(param) { type_ = Engine::Collision2D::Type::Sprite; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision2D::Sprite* param_ = nullptr;
};