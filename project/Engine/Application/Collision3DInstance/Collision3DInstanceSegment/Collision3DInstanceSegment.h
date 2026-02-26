#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstanceSegment : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstanceSegment(Engine::Collision3D::Segment* param) : param_(param) { type_ = Engine::Collision3D::Type::Segment; }

	/// @brief パラメータを取得する
	/// @return 
	void* GetParam() override { return param_; }

	/// @brief パラメータ
	Engine::Collision3D::Segment* param_ = nullptr;
};

