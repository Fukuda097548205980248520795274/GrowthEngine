#pragma once
#include "../BaseCollisionInstance.h"

class CollisionInstanceRay : public Engine::BaseCollisionInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	CollisionInstanceRay(Engine::Collision::Ray* param) : param_(param) { type_ = Engine::Collision::Type::Ray; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision::Ray* param_ = nullptr;
};

