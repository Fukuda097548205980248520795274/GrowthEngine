#pragma once
#define NOMINMAX
#include "Data/CollisionData/CollisionData.h"

namespace Engine
{
	/// @brief 衝突判定
	/// @param s1 球1
	/// @param s2 球2
	/// @return 
	bool CollisionCheckFunc(const Collision3D::Sphere& s1, const Collision3D::Sphere& s2);

	/// @brief 衝突判定
	/// @param aabb1 AABB1
	/// @param aabb2 AABB2
	/// @return 
	bool CollisionCheckFunc(const Collision3D::AABB& aabb1, const Collision3D::AABB& aabb2);

	/// @brief 衝突判定
	/// @param obb1 OBB1
	/// @param obb2 OBB2
	/// @return 
	bool CollisionCheckFunc(const Collision3D::OBB& obb1, const Collision3D::OBB& obb2);

	/// @brief 衝突判定
	/// @param sphere 球
	/// @param aabb AABB
	/// @return 
	bool CollisionCheckFunc(const Collision3D::Sphere& sphere, const Collision3D::AABB& aabb);

	/// @brief 衝突判定
	/// @param sphere 球
	/// @param obb OBB
	/// @return 
	bool CollisionCheckFunc(const Collision3D::Sphere& sphere, const Collision3D::OBB& obb);

	/// @brief 衝突判定
	/// @param aabb AABB
	/// @param obb OBB
	/// @return 
	bool CollisionCheckFunc(const Collision3D::AABB& aabb, const Collision3D::OBB& obb);

	/// @brief 衝突判定
	/// @param plane 平面
	/// @param sphere 球
	/// @return 
	bool CollisionCheckFunc(const Collision3D::Plane& plane, const Collision3D::Sphere& sphere);

	/// @brief 衝突判定
	/// @param plane 平面
	/// @param line 直線
	/// @return 
	bool CollisionCheckFunc(const Collision3D::Plane& plane, const Collision3D::Line& line);

	/// @brief 衝突判定
	/// @param plane 平面
	/// @param ray 半直線
	/// @return 
	bool CollisionCheckFunc(const Collision3D::Plane& plane, const Collision3D::Ray& ray);

	/// @brief 衝突判定
	/// @param plane 平面
	/// @param segment 線分
	/// @return 
	bool CollisionCheckFunc(const Collision3D::Plane& plane, const Collision3D::Segment& segment);

	/// @brief 衝突判定
	/// @param aabb AABB
	/// @param line 直線
	/// @return 
	bool CollisionCheckFunc(const Collision3D::AABB& aabb, const Collision3D::Line& line);

	/// @brief 衝突判定
	/// @param aabb AABB
	/// @param ray 半直線
	/// @return 
	bool CollisionCheckFunc(const Collision3D::AABB& aabb, const Collision3D::Ray& ray);

	/// @brief 衝突判定
	/// @param aabb AABB
	/// @param segment 線分
	/// @return 
	bool CollisionCheckFunc(const Collision3D::AABB& aabb, const Collision3D::Segment& segment);

	/// @brief 衝突判定
	/// @param obb OBB
	/// @param line 直線
	/// @return 
	bool CollisionCheckFunc(const Collision3D::OBB& obb, const Collision3D::Line& line);

	/// @brief 衝突判定
	/// @param obb OBB
	/// @param ray 半直線
	/// @return 
	bool CollisionCheckFunc(const Collision3D::OBB& obb, const Collision3D::Ray& ray);

	/// @brief 衝突判定
	/// @param obb OBB
	/// @param segment 線分
	/// @return 
	bool CollisionCheckFunc(const Collision3D::OBB& obb, const Collision3D::Segment& segment);
}