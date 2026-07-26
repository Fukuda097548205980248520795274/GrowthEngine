#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstanceAABB : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstanceAABB(Engine::Collision3D::AABB* param);

	/// @brief パラメータ
	std::unique_ptr<Engine::Collision3D::AABB> param_ = nullptr;
};