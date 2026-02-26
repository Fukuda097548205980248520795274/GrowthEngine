#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstancePlane : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstancePlane(Engine::Collision3D::Plane* param) : param_(param) { type_ = Engine::Collision3D::Type::Plane; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision3D::Plane* param_ = nullptr;
};

