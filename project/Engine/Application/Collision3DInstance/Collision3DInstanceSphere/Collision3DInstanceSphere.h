#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstanceSphere : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstanceSphere(Engine::Collision3D::Sphere* param);

	/// @brief パラメータ
	std::unique_ptr<Engine::Collision3D::Sphere> param_ = nullptr;
};

