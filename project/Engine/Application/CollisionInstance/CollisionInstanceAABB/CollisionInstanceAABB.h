#pragma once
#include "../BaseCollisionInstance.h"

class CollisionInstanceAABB : public Engine::BaseCollisionInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	CollisionInstanceAABB(Engine::Collision3D::AABB* param) : param_(param) { type_ = Engine::Collision3D::Type::AABB; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision3D::AABB* param_ = nullptr;
};