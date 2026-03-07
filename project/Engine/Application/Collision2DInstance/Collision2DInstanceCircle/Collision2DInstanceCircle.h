#pragma once
#include "../BaseCollision2DInstance.h"

class Collision2DInstanceCircle : public Engine::BaseCollision2DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision2DInstanceCircle(Engine::Collision2D::Circle* param);

	/// @brief パラメータ
	std::unique_ptr<Engine::Collision2D::Circle> param_ = nullptr;
};