#pragma once
#include "../BaseCollision2DInstance.h"

class Collision2DInstanceCircle : public Engine::BaseCollision2DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision2DInstanceCircle(Engine::Collision2D::Circle* param) : param_(param) { type_ = Engine::Collision2D::Type::Circle; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision2D::Circle* param_ = nullptr;
};