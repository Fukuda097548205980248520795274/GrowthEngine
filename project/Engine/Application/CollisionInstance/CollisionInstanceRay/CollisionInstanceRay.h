#pragma once
#include "../BaseCollisionInstance.h"

class CollisionInstanceRay : public Engine::BaseCollisionInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	CollisionInstanceRay(Engine::Collision3D::Ray* param) : param_(param) { type_ = Engine::Collision3D::Type::Ray; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision3D::Ray* param_ = nullptr;
};

