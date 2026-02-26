#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstanceLine : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstanceLine(Engine::Collision3D::Line* param) : param_(param) { type_ = Engine::Collision3D::Type::Line; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision3D::Line* param_ = nullptr;
};

