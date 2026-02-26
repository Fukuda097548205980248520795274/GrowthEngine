#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstanceRay : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstanceRay(Engine::Collision3D::Ray* param) : param_(param) { type_ = Engine::Collision3D::Type::Ray; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision3D::Ray* param_ = nullptr;
};

