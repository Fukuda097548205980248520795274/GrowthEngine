#pragma once
#include "../BaseCollisionInstance.h"

class CollisionInstancePlane : public Engine::BaseCollisionInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	CollisionInstancePlane(Engine::Collision3D::Plane* param) : param_(param) { type_ = Engine::Collision3D::Type::Plane; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision3D::Plane* param_ = nullptr;
};

