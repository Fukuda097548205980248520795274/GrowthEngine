#pragma once
#include "../BaseCollision3DInstance.h"

class Collision3DInstanceSegment : public Engine::BaseCollision3DInstance
{
public:

	/// @brief コンストラクタ
	/// @param param 
	Collision3DInstanceSegment(Engine::Collision3D::Segment* param);

	/// @brief パラメータ
	std::unique_ptr<Engine::Collision3D::Segment> param_ = nullptr;
};

