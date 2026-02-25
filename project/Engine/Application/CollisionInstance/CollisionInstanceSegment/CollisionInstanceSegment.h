#pragma once
#include "../BaseCollisionInstance.h"

class CollisionInstanceSegment : public Engine::BaseCollisionInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	CollisionInstanceSegment(Engine::Collision3D::Segment* param) : param_(param) { type_ = Engine::Collision3D::Type::Segment; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision3D::Segment* param_ = nullptr;
};

