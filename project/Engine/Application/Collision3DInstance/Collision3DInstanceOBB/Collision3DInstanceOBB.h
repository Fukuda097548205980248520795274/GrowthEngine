#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstanceOBB : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstanceOBB(Engine::Collision3D::OBB* param) : param_(param) { type_ = Engine::Collision3D::Type::OBB; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision3D::OBB* param_ = nullptr;
};

