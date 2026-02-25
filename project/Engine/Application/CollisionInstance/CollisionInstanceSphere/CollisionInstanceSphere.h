#pragma once
#include "../BaseCollisionInstance.h"

class CollisionInstanceSphere : public Engine::BaseCollisionInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	CollisionInstanceSphere(Engine::Collision3D::Sphere* param) : param_(param) { type_ = Engine::Collision3D::Type::Sphere; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision3D::Sphere* param_ = nullptr;
};

