#pragma once
#include "../BaseCollisionInstance.h"

class CollisionInstanceOBB : public Engine::BaseCollisionInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	CollisionInstanceOBB(Engine::Collision::OBB* param) : param_(param) { type_ = Engine::Collision::Type::OBB; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision::OBB* param_ = nullptr;
};

