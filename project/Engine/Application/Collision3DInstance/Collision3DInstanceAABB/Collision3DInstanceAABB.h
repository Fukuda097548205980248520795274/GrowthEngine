#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstanceAABB : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstanceAABB(Engine::Collision3D::AABB* param) : param_(param) { type_ = Engine::Collision3D::Type::AABB; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision3D::AABB* param_ = nullptr;
};