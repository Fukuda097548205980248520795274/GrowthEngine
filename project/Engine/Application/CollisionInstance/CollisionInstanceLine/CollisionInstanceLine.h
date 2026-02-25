#pragma once
#include "../BaseCollisionInstance.h"

class CollisionInstanceLine : public Engine::BaseCollisionInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	CollisionInstanceLine(Engine::Collision3D::Line* param) : param_(param) { type_ = Engine::Collision3D::Type::Line; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision3D::Line* param_ = nullptr;
};

