#pragma once
#include "../BaseCollisionInstance.h"

class CollisionInstanceLine : public Engine::BaseCollisionInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	CollisionInstanceLine(Engine::Collision::Line* param) : param_(param) { type_ = Engine::Collision::Type::Line; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision::Line* param_ = nullptr;
};

