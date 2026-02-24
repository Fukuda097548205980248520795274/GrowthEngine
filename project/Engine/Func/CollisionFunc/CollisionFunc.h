#pragma once
#include "Data/CollisionData/CollisionData.h"

namespace Engine
{
	/// @brief 衝突判定
	/// @param aabb1 
	/// @param aabb2 
	/// @return 
	bool CollisionCheckFunc(const Collision::AABB& aabb1, const Collision::AABB& aabb2);
}