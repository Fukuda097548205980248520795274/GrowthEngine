#pragma once
#include "../BaseCollisionInstance.h"

class CollisionInstancePlane : public Engine::BaseCollisionInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	CollisionInstancePlane(Engine::Collision::Plane* param) : param_(param) { type_ = Engine::Collision::Type::Plane; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision::Plane* param_ = nullptr;
};

