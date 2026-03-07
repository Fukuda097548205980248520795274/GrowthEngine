#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstanceRay : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstanceRay(Engine::Collision3D::Ray* param);

	/// @brief パラメータ
	std::unique_ptr<Engine::Collision3D::Ray> param_ = nullptr;
};

