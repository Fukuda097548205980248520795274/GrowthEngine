#include "CollisionFunc.h"

/// @brief 衝突判定
/// @param aabb1 
/// @param aabb2 
/// @return 
bool Engine::CollisionCheckFunc(const Collision::AABB& aabb1, const Collision::AABB& aabb2)
{
	if (aabb1.center.x + aabb1.radius.x >= aabb2.center.x - aabb2.center.x &&
		aabb1.center.x - aabb1.radius.x <= aabb2.center.x + aabb2.center.x)
		if (aabb1.center.y + aabb1.radius.y >= aabb2.center.y - aabb2.center.y &&
			aabb1.center.y - aabb1.radius.y <= aabb2.center.y + aabb2.center.y)
			if (aabb1.center.z + aabb1.radius.z >= aabb2.center.z - aabb2.center.z &&
				aabb1.center.z - aabb1.radius.z <= aabb2.center.z + aabb2.center.z)
				return true;

	return false;
}