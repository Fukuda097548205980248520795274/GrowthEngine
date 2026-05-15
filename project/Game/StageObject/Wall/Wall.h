#pragma once
#include "../StageObject.h"

struct WallInitData
{
	/// @brief 壁の位置
	Vector3 position = Vector3(0.0f, 0.0f, 0.0f);

	// 壁の衝突判定
	Collision3DInstanceSegment* collision = nullptr;
};

class Wall : public StageObject
{

};