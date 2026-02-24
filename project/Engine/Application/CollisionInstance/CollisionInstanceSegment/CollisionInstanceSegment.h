#pragma once
#include "../BaseCollisionInstance.h"

class CollisionInstanceSegment : public Engine::BaseCollisionInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	CollisionInstanceSegment(Engine::Collision::Segment* param) : param_(param) { type_ = Engine::Collision::Type::Segment; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision::Segment* param_ = nullptr;
};

