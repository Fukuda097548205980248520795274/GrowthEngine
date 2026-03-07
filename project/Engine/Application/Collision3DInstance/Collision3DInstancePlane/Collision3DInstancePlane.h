#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstancePlane : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstancePlane(Engine::Collision3D::Plane* param);

	/// @brief パラメータ
	std::unique_ptr<Engine::Collision3D::Plane> param_ = nullptr;
};

