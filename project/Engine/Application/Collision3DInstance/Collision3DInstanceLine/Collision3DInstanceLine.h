#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstanceLine : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstanceLine(Engine::Collision3D::Line* param);

	/// @brief パラメータ
	std::unique_ptr<Engine::Collision3D::Line> param_ = nullptr;
};

