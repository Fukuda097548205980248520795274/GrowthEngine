#pragma once
#include "../BaseCollisionInstance.h"

class CollisionInstanceSphere : public Engine::BaseCollisionInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	CollisionInstanceSphere(Engine::Collision::Sphere* param) : param_(param) { type_ = Engine::Collision::Type::Sphere; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision::Sphere* param_ = nullptr;
};

