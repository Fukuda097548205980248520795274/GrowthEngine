#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstanceOBB : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstanceOBB(Engine::Collision3D::OBB* param);

	/// @brief パラメータ
	std::unique_ptr<Engine::Collision3D::OBB> param_ = nullptr;
};

