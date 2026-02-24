#include "CollisionFunc.h"
#include <algorithm>
#include <cmath>
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

/// @brief 衝突判定
/// @param s1 球1
/// @param s2 球2
/// @return 
bool Engine::CollisionCheckFunc(const Collision::Sphere& s1, const Collision::Sphere& s2)
{
	if (s1.radius + s2.radius >= (s2.center - s1.center).Length())
		return true;

	return false;
}

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

/// @brief 衝突判定
/// @param obb1 OBB1
/// @param obb2 OBB2
/// @return 
bool Engine::CollisionCheckFunc(const Collision::OBB& obb1, const Collision::OBB& obb2)
{
	/*---------------------- -
		ワールド座標を求める
	-----------------------*/

		/*   OBB1   */

	// ワールド行列
	Matrix4x4 obbWorld1;

	obbWorld1.m[0][0] = obb1.oriented[0].x;
	obbWorld1.m[0][1] = obb1.oriented[0].y;
	obbWorld1.m[0][2] = obb1.oriented[0].z;
	obbWorld1.m[0][3] = 0.0f;

	obbWorld1.m[1][0] = obb1.oriented[1].x;
	obbWorld1.m[1][1] = obb1.oriented[1].y;
	obbWorld1.m[1][2] = obb1.oriented[1].z;
	obbWorld1.m[1][3] = 0.0f;

	obbWorld1.m[2][0] = obb1.oriented[2].x;
	obbWorld1.m[2][1] = obb1.oriented[2].y;
	obbWorld1.m[2][2] = obb1.oriented[2].z;
	obbWorld1.m[2][3] = 0.0f;

	obbWorld1.m[3][0] = obb1.center.x;
	obbWorld1.m[3][1] = obb1.center.y;
	obbWorld1.m[3][2] = obb1.center.z;
	obbWorld1.m[3][3] = 1.0f;


	// ローカル座標
	Vector3 local1[8];
	local1[0] = { -obb1.radius.x , -obb1.radius.y , -obb1.radius.z };
	local1[1] = { obb1.radius.x , -obb1.radius.y , -obb1.radius.z };
	local1[2] = { -obb1.radius.x , -obb1.radius.y , obb1.radius.z };
	local1[3] = { obb1.radius.x , -obb1.radius.y , obb1.radius.z };
	local1[4] = { -obb1.radius.x , obb1.radius.y , -obb1.radius.z };
	local1[5] = { obb1.radius.x , obb1.radius.y , -obb1.radius.z };
	local1[6] = { -obb1.radius.x , obb1.radius.y , obb1.radius.z };
	local1[7] = { obb1.radius.x , obb1.radius.y , obb1.radius.z };


	// ワールド座標
	Vector3 world1[8];
	for (uint32_t i = 0; i < 8; i++)
	{
		world1[i] = Transform(local1[i], obbWorld1);
	}


	/*   OBB2   */

	// ワールド行列
	Matrix4x4 obbWorld2;

	obbWorld2.m[0][0] = obb2.oriented[0].x;
	obbWorld2.m[0][1] = obb2.oriented[0].y;
	obbWorld2.m[0][2] = obb2.oriented[0].z;
	obbWorld2.m[0][3] = 0.0f;

	obbWorld2.m[1][0] = obb2.oriented[1].x;
	obbWorld2.m[1][1] = obb2.oriented[1].y;
	obbWorld2.m[1][2] = obb2.oriented[1].z;
	obbWorld2.m[1][3] = 0.0f;

	obbWorld2.m[2][0] = obb2.oriented[2].x;
	obbWorld2.m[2][1] = obb2.oriented[2].y;
	obbWorld2.m[2][2] = obb2.oriented[2].z;
	obbWorld2.m[2][3] = 0.0f;

	obbWorld2.m[3][0] = obb2.center.x;
	obbWorld2.m[3][1] = obb2.center.y;
	obbWorld2.m[3][2] = obb2.center.z;
	obbWorld2.m[3][3] = 1.0f;


	// ローカル座標
	Vector3 local2[8];
	local2[0] = { -obb2.radius.x , -obb2.radius.y , -obb2.radius.z };
	local2[1] = { obb2.radius.x , -obb2.radius.y , -obb2.radius.z };
	local2[2] = { -obb2.radius.x , -obb2.radius.y , obb2.radius.z };
	local2[3] = { obb2.radius.x , -obb2.radius.y , obb2.radius.z };
	local2[4] = { -obb2.radius.x , obb2.radius.y , -obb2.radius.z };
	local2[5] = { obb2.radius.x , obb2.radius.y , -obb2.radius.z };
	local2[6] = { -obb2.radius.x , obb2.radius.y , obb2.radius.z };
	local2[7] = { obb2.radius.x , obb2.radius.y , obb2.radius.z };


	// ワールド座標
	Vector3 world2[8];
	for (uint32_t i = 0; i < 8; i++)
	{
		world2[i] = Transform(local2[i], obbWorld2);
	}


	/*------------
		分離軸
	------------*/

	/*   面法線 OBB1 X   */

	Vector3 normal = obb1.oriented[0];

	float min1 = Dot(world1[0], normal);
	float max1 = Dot(world1[0], normal);

	float min2 = Dot(world2[0], normal);
	float max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	float sumSpan = (max1 - min1) + (max2 - min2);
	float longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   面法線 OBB1 Y   */

	normal = obb1.oriented[1];

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   面法線 OBB1 Z   */

	normal = obb1.oriented[2];

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   面法線 OBB2 X   */

	normal = obb2.oriented[0];

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   面法線 OBB2 Y   */

	normal = obb2.oriented[1];

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   面法線 OBB2 Z   */

	normal = obb2.oriented[2];

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 OBB1 : X , OBB2 : X   */

	normal = (Cross(obb1.oriented[0], obb2.oriented[0])).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 OBB1 : X , OBB2 : Y   */

	normal = (Cross(obb1.oriented[0], obb2.oriented[1])).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 OBB1 : X , OBB2 : Z   */

	normal = (Cross(obb1.oriented[0], obb2.oriented[2])).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 OBB1 : Y , OBB2 : X   */

	normal = (Cross(obb1.oriented[1], obb2.oriented[0])).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 OBB1 : Y , OBB2 : Y   */

	normal = (Cross(obb1.oriented[1], obb2.oriented[1])).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 OBB1 : Y , OBB2 : Z   */

	normal = (Cross(obb1.oriented[1], obb2.oriented[2])).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 OBB1 : Z , OBB2 : X   */

	normal = (Cross(obb1.oriented[2], obb2.oriented[0])).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 OBB1 : Z , OBB2 : Y   */

	normal = (Cross(obb1.oriented[2], obb2.oriented[1])).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 OBB1 : Z , OBB2 : Z   */

	normal = (Cross(obb1.oriented[2], obb2.oriented[2])).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	return true;
}

/// @brief 衝突判定
/// @param sphere 球
/// @param aabb AABB
/// @return 
bool Engine::CollisionCheckFunc(const Collision::Sphere& sphere, const Collision::AABB& aabb)
{
	// 最近接点
	Vector3 closetPoint;
	closetPoint.x = std::clamp(sphere.center.x, aabb.center.x - aabb.radius.x, aabb.center.x + aabb.radius.x);
	closetPoint.y = std::clamp(sphere.center.y, aabb.center.y - aabb.radius.y, aabb.center.y + aabb.radius.y);
	closetPoint.z = std::clamp(sphere.center.z, aabb.center.z - aabb.radius.z, aabb.center.z + aabb.radius.z);

	// 最近接点が半径より小さいとき
	if (sphere.radius >= (sphere.center - closetPoint).Length())
		return true;

	return false;
}

/// @brief 衝突判定
/// @param sphere 球
/// @param obb OBB
/// @return 
bool Engine::CollisionCheckFunc(const Collision::Sphere& sphere, const Collision::OBB& obb)
{
	// ワールド行列
	Matrix4x4 obbWorld;

	obbWorld.m[0][0] = obb.oriented[0].x;
	obbWorld.m[0][1] = obb.oriented[0].y;
	obbWorld.m[0][2] = obb.oriented[0].z;
	obbWorld.m[0][3] = 0.0f;

	obbWorld.m[1][0] = obb.oriented[1].x;
	obbWorld.m[1][1] = obb.oriented[1].y;
	obbWorld.m[1][2] = obb.oriented[1].z;
	obbWorld.m[1][3] = 0.0f;

	obbWorld.m[2][0] = obb.oriented[2].x;
	obbWorld.m[2][1] = obb.oriented[2].y;
	obbWorld.m[2][2] = obb.oriented[2].z;
	obbWorld.m[2][3] = 0.0f;

	obbWorld.m[3][0] = obb.center.x;
	obbWorld.m[3][1] = obb.center.y;
	obbWorld.m[3][2] = obb.center.z;
	obbWorld.m[3][3] = 1.0f;


	// OBBのワールド逆行列
	Matrix4x4 inverseMatrix = obbWorld.Inverse();

	// 球をOBBの座標に持ち込む
	Vector3 centerInOBBLocalSpace = Transform(sphere.center, inverseMatrix);

	// OBB視点のため、AABBを使用する
	Collision::AABB aabbOBBLocal;
	aabbOBBLocal.center = Vector3(0.0f, 0.0f, 0.0f);
	aabbOBBLocal.radius = obb.radius;

	// OBB視点の球
	Collision::Sphere sphereOBBLocal =
	{
		centerInOBBLocalSpace,
		sphere.radius
	};

	// AABBと球の当たり判定
	return CollisionCheckFunc(sphereOBBLocal,aabbOBBLocal);
}

/// @brief 衝突判定
/// @param aabb AABB
/// @param obb OBB
/// @return 
bool Engine::CollisionCheckFunc(const Collision::AABB& aabb, const Collision::OBB& obb)
{
	/*-----------------------
		ワールド座標を求める
	-----------------------*/

	/*   AABB   */

	// ワールド行列
	Matrix4x4 aabbWorld;

	aabbWorld.m[0][0] = 1.0f;
	aabbWorld.m[0][1] = 0.0f;
	aabbWorld.m[0][2] = 0.0f;
	aabbWorld.m[0][3] = 0.0f;

	aabbWorld.m[1][0] = 0.0f;
	aabbWorld.m[1][1] = 1.0f;
	aabbWorld.m[1][2] = 0.0f;
	aabbWorld.m[1][3] = 0.0f;

	aabbWorld.m[2][0] = 0.0f;
	aabbWorld.m[2][1] = 0.0f;
	aabbWorld.m[2][2] = 1.0f;
	aabbWorld.m[2][3] = 0.0f;

	Vector3 aabbMax = aabb.center + aabb.radius;
	Vector3 aabbMin = aabb.center - aabb.radius;

	aabbWorld.m[3][0] = ((aabbMax.x - aabbMin.x) / 2.0f) + aabbMin.x;
	aabbWorld.m[3][1] = ((aabbMax.y - aabbMin.y) / 2.0f) + aabbMin.y;
	aabbWorld.m[3][2] = ((aabbMax.z - aabbMin.z) / 2.0f) + aabbMin.z;
	aabbWorld.m[3][3] = 1.0f;

	// 半径
	Vector3 aabbRadius = aabbMax - Vector3(aabbWorld.m[3][0], aabbWorld.m[3][1], aabbWorld.m[3][2]);

	// ローカル座標
	Vector3 local1[8];
	local1[0] = { -aabbRadius.x , -aabbRadius.y , -aabbRadius.z };
	local1[1] = { aabbRadius.x , -aabbRadius.y , -aabbRadius.z };
	local1[2] = { -aabbRadius.x , -aabbRadius.y , aabbRadius.z };
	local1[3] = { aabbRadius.x , -aabbRadius.y , aabbRadius.z };
	local1[4] = { -aabbRadius.x , aabbRadius.y , -aabbRadius.z };
	local1[5] = { aabbRadius.x , aabbRadius.y , -aabbRadius.z };
	local1[6] = { -aabbRadius.x , aabbRadius.y , aabbRadius.z };
	local1[7] = { aabbRadius.x , aabbRadius.y , aabbRadius.z };


	// ワールド座標
	Vector3 world1[8];
	for (uint32_t i = 0; i < 8; i++)
	{
		world1[i] = Transform(local1[i], aabbWorld);
	}


	/*   OBB   */

	// ワールド行列
	Matrix4x4 obbWorld;

	obbWorld.m[0][0] = obb.oriented[0].x;
	obbWorld.m[0][1] = obb.oriented[0].y;
	obbWorld.m[0][2] = obb.oriented[0].z;
	obbWorld.m[0][3] = 0.0f;

	obbWorld.m[1][0] = obb.oriented[1].x;
	obbWorld.m[1][1] = obb.oriented[1].y;
	obbWorld.m[1][2] = obb.oriented[1].z;
	obbWorld.m[1][3] = 0.0f;

	obbWorld.m[2][0] = obb.oriented[2].x;
	obbWorld.m[2][1] = obb.oriented[2].y;
	obbWorld.m[2][2] = obb.oriented[2].z;
	obbWorld.m[2][3] = 0.0f;

	obbWorld.m[3][0] = obb.center.x;
	obbWorld.m[3][1] = obb.center.y;
	obbWorld.m[3][2] = obb.center.z;
	obbWorld.m[3][3] = 1.0f;


	// ローカル座標
	Vector3 local2[8];
	local2[0] = { -obb.radius.x , -obb.radius.y , -obb.radius.z };
	local2[1] = { obb.radius.x , -obb.radius.y , -obb.radius.z };
	local2[2] = { -obb.radius.x , -obb.radius.y , obb.radius.z };
	local2[3] = { obb.radius.x , -obb.radius.y , obb.radius.z };
	local2[4] = { -obb.radius.x , obb.radius.y , -obb.radius.z };
	local2[5] = { obb.radius.x , obb.radius.y , -obb.radius.z };
	local2[6] = { -obb.radius.x , obb.radius.y , obb.radius.z };
	local2[7] = { obb.radius.x , obb.radius.y , obb.radius.z };


	// ワールド座標
	Vector3 world2[8];
	for (uint32_t i = 0; i < 8; i++)
	{
		world2[i] = Transform(local2[i], obbWorld);
	}


	/*------------
		分離軸
	------------*/

	/*   面法線 AABB X   */

	Vector3 normal = Vector3(1.0f, 0.0f, 0.0f);

	float min1 = Dot(world1[0], normal);
	float max1 = Dot(world1[0], normal);

	float min2 = Dot(world2[0], normal);
	float max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	float sumSpan = (max1 - min1) + (max2 - min2);
	float longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   面法線 AABB Y   */

	normal = Vector3(0.0f, 1.0f, 0.0f);

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   面法線 AABB Z   */

	normal = Vector3(0.0f, 0.0f, 1.0f);

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   面法線 OBB X   */

	normal = obb.oriented[0];

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   面法線 OBB Y   */

	normal = obb.oriented[1];

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   面法線 OBB Z   */

	normal = obb.oriented[2];

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 AABB : X , OBB : X   */

	normal = Cross(Vector3(1.0f, 0.0f, 0.0f), obb.oriented[0]).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 AABB : X , OBB : Y   */

	normal = Cross(Vector3(1.0f, 0.0f, 0.0f), obb.oriented[1]).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 AABB : X , OBB : Z   */

	normal = Cross(Vector3(1.0f, 0.0f, 0.0f), obb.oriented[2]).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 AABB : Y , OBB : X   */

	normal = Cross(Vector3(0.0f, 1.0f, 0.0f), obb.oriented[0]).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 AABB : Y , OBB : Y   */

	normal = Cross(Vector3(0.0f, 1.0f, 0.0f), obb.oriented[1]).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 AABB : Y , OBB : Z   */

	normal = Cross(Vector3(0.0f, 1.0f, 0.0f), obb.oriented[2]).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 AABB : Z , OBB : X   */

	normal = Cross(Vector3(0.0f, 0.0f, 1.0f), obb.oriented[0]).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 AABB : Z , OBB : Y   */

	normal = Cross(Vector3(0.0f, 0.0f, 1.0f), obb.oriented[1]).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	/*   クロス積 AABB : Z , OBB : Z   */

	normal = Cross(Vector3(0.0f, 0.0f, 1.0f), obb.oriented[2]).Normalize();

	min1 = Dot(world1[0], normal);
	max1 = Dot(world1[0], normal);

	min2 = Dot(world2[0], normal);
	max2 = Dot(world2[0], normal);

	for (uint32_t i = 1; i < 8; i++)
	{
		float proj1 = Dot(world1[i], normal);
		float proj2 = Dot(world2[i], normal);

		min1 = std::min(min1, proj1);
		max1 = std::max(max1, proj1);

		min2 = std::min(min2, proj2);
		max2 = std::max(max2, proj2);
	}

	sumSpan = (max1 - min1) + (max2 - min2);
	longSpan = (std::max)(max1, max2) - (std::min)(min1, min2);

	if (sumSpan < longSpan)
		return false;


	return true;
}

/// @brief 衝突判定
/// @param plane 平面
/// @param sphere 球
/// @return 
bool Engine::CollisionCheckFunc(const Collision::Plane& plane, const Collision::Sphere& sphere)
{
	// 平面方程式で距離を求める
	float distance = Dot(sphere.center, plane.normal) - plane.distance;

	// 距離が半径より小さいとき
	if (std::fabs(distance) <= sphere.radius)
		return true;

	return false;
}

/// @brief 衝突判定
/// @param plane 平面
/// @param line 直線
/// @return 
bool Engine::CollisionCheckFunc(const Collision::Plane& plane, const Collision::Line& line)
{
	// 法線と終点方向の内積
	float NdotD = Dot(plane.normal, line.diff);

	// 垂直の場合当たらない
	if (NdotD == 0.0f)
		return false;

	return true;
}

/// @brief 衝突判定
/// @param plane 平面
/// @param ray 半直線
/// @return 
bool Engine::CollisionCheckFunc(const Collision::Plane& plane, const Collision::Ray& ray)
{
	// 法線と終点方向の内積
	float NdotD = Dot(plane.normal, ray.diff);

	// 垂直の場合当たらない
	if (NdotD == 0.0f)
		return false;

	// 法線と始点の内積
	float NdotO = Dot(plane.normal, ray.start);

	// 媒介変数を求める
	float t = (plane.distance - NdotO) / NdotD;

	if (t >= 0.0f)
		return true;

	return false;
}

/// @brief 衝突判定
/// @param plane 平面
/// @param segment 線分
/// @return 
bool Engine::CollisionCheckFunc(const Collision::Plane& plane, const Collision::Segment& segment)
{
	// 法線と終点方向の内積
	float NdotD = Dot(plane.normal, segment.diff);

	// 垂直の場合は当たらない
	if (NdotD == 0.0f)
		return false;

	// 法線と始点の内積
	float NdotO = Dot(plane.normal, segment.start);

	// 媒介変数を求める
	float t = (plane.distance - NdotO) / NdotD;

	if (t >= 0.0f && t <= 1.0f)
		return true;

	return false;
}

/// @brief 衝突判定
/// @param aabb AABB
/// @param line 直線
/// @return 
bool Engine::CollisionCheckFunc(const Collision::AABB& aabb, const Collision::Line& line)
{
	Vector3 tMin =
	{
		((aabb.center.x - aabb.radius.x) - line.start.x) / line.diff.x,
		((aabb.center.y - aabb.radius.y) - line.start.y) / line.diff.y,
		((aabb.center.z - aabb.radius.z) - line.start.z) / line.diff.z
	};

	Vector3 tMax =
	{
		((aabb.center.x + aabb.radius.x) - line.start.x) / line.diff.x,
		((aabb.center.y + aabb.radius.y) - line.start.y) / line.diff.y,
		((aabb.center.z + aabb.radius.z) - line.start.z) / line.diff.z
	};

	Vector3 tNear =
	{
		std::min(tMin.x , tMax.x),
		std::min(tMin.y , tMax.y),
		std::min(tMin.z , tMax.z)
	};

	Vector3 tFar =
	{
		std::max(tMin.x , tMax.x),
		std::max(tMin.y , tMax.y),
		std::max(tMin.z , tMax.z)
	};

	float tmin = std::max(std::max(tNear.x, tNear.y), tNear.z);
	float tmax = std::min(std::min(tFar.x, tFar.y), tFar.z);

	if (tmin <= tmax)
		return true;

	return false;
}

/// @brief 衝突判定
/// @param aabb AABB
/// @param ray 半直線
/// @return 
bool Engine::CollisionCheckFunc(const Collision::AABB& aabb, const Collision::Ray& ray)
{
	Vector3 tMin =
	{
		((aabb.center.x - aabb.radius.x) - ray.start.x) / ray.diff.x,
		((aabb.center.y - aabb.radius.y) - ray.start.y) / ray.diff.y,
		((aabb.center.z - aabb.radius.z) - ray.start.z) / ray.diff.z
	};

	Vector3 tMax =
	{
		((aabb.center.x + aabb.radius.x) - ray.start.x) / ray.diff.x,
		((aabb.center.y + aabb.radius.y) - ray.start.y) / ray.diff.y,
		((aabb.center.z + aabb.radius.z) - ray.start.z) / ray.diff.z
	};

	Vector3 tNear =
	{
		std::min(tMin.x , tMax.x),
		std::min(tMin.y , tMax.y),
		std::min(tMin.z , tMax.z)
	};

	Vector3 tFar =
	{
		std::max(tMin.x , tMax.x),
		std::max(tMin.y , tMax.y),
		std::max(tMin.z , tMax.z)
	};

	float tmin = std::max(std::max(tNear.x, tNear.y), tNear.z);
	float tmax = std::min(std::min(tFar.x, tFar.y), tFar.z);

	if (tmax >= 0.0f)if (tmin <= tmax)
		return true;

	return false;
}

/// @brief 衝突判定
/// @param aabb AABB
/// @param segment 線分
/// @return 
bool Engine::CollisionCheckFunc(const Collision::AABB& aabb, const Collision::Segment& segment)
{
	Vector3 tMin =
	{
		((aabb.center.x - aabb.radius.x) - segment.start.x) / segment.diff.x,
		((aabb.center.y - aabb.radius.y) - segment.start.y) / segment.diff.y,
		((aabb.center.z - aabb.radius.z) - segment.start.z) / segment.diff.z
	};

	Vector3 tMax =
	{
		((aabb.center.x + aabb.radius.x) - segment.start.x) / segment.diff.x,
		((aabb.center.y + aabb.radius.y) - segment.start.y) / segment.diff.y,
		((aabb.center.z + aabb.radius.z) - segment.start.z) / segment.diff.z
	};

	Vector3 tNear =
	{
		std::min(tMin.x , tMax.x),
		std::min(tMin.y , tMax.y),
		std::min(tMin.z , tMax.z)
	};

	Vector3 tFar =
	{
		std::max(tMin.x , tMax.x),
		std::max(tMin.y , tMax.y),
		std::max(tMin.z , tMax.z)
	};

	float tmin = std::max(std::max(tNear.x, tNear.y), tNear.z);
	float tmax = std::min(std::min(tFar.x, tFar.y), tFar.z);

	if (tmax >= 0.0f && tmin <= 1.0f)if (tmin <= tmax)
		return true;

	return false;
}

/// @brief 衝突判定
/// @param obb OBB
/// @param line 直線
/// @return 
bool Engine::CollisionCheckFunc(const Collision::OBB& obb, const Collision::Line& line)
{
	// OBBのワールド行列
	Matrix4x4 obbWorld;

	obbWorld.m[0][0] = obb.oriented[0].x;
	obbWorld.m[0][1] = obb.oriented[0].y;
	obbWorld.m[0][2] = obb.oriented[0].z;
	obbWorld.m[0][3] = 0.0f;

	obbWorld.m[1][0] = obb.oriented[1].x;
	obbWorld.m[1][1] = obb.oriented[1].y;
	obbWorld.m[1][2] = obb.oriented[1].z;
	obbWorld.m[1][3] = 0.0f;

	obbWorld.m[2][0] = obb.oriented[2].x;
	obbWorld.m[2][1] = obb.oriented[2].y;
	obbWorld.m[2][2] = obb.oriented[2].z;
	obbWorld.m[2][3] = 0.0f;

	obbWorld.m[3][0] = obb.center.x;
	obbWorld.m[3][1] = obb.center.y;
	obbWorld.m[3][2] = obb.center.z;
	obbWorld.m[3][3] = 1.0f;


	// 逆行列
	Matrix4x4 obbInverse = obbWorld.Inverse();

	// OBBのワールド座標に持ち込む
	Vector3 localOrigin = Transform(line.start, obbInverse);
	Vector3 localEnd = Transform(line.start + line.diff, obbInverse);

	// OBB視点のためAABBを使用する
	Collision::AABB localAABB;
	localAABB.center = Vector3(0.0f, 0.0f, 0.0f);
	localAABB.radius = obb.radius;

	// OBB視点の直線
	Collision::Line localLine;
	localLine.start = localOrigin;
	localLine.diff = localEnd - localOrigin;

	// AABBと直線の当たり判定
	return CollisionCheckFunc(localAABB, localLine);
}

/// @brief 衝突判定
/// @param obb OBB
/// @param ray 半直線
/// @return 
bool Engine::CollisionCheckFunc(const Collision::OBB& obb, const Collision::Ray& ray)
{
	// OBBのワールド行列
	Matrix4x4 obbWorld;

	obbWorld.m[0][0] = obb.oriented[0].x;
	obbWorld.m[0][1] = obb.oriented[0].y;
	obbWorld.m[0][2] = obb.oriented[0].z;
	obbWorld.m[0][3] = 0.0f;

	obbWorld.m[1][0] = obb.oriented[1].x;
	obbWorld.m[1][1] = obb.oriented[1].y;
	obbWorld.m[1][2] = obb.oriented[1].z;
	obbWorld.m[1][3] = 0.0f;

	obbWorld.m[2][0] = obb.oriented[2].x;
	obbWorld.m[2][1] = obb.oriented[2].y;
	obbWorld.m[2][2] = obb.oriented[2].z;
	obbWorld.m[2][3] = 0.0f;

	obbWorld.m[3][0] = obb.center.x;
	obbWorld.m[3][1] = obb.center.y;
	obbWorld.m[3][2] = obb.center.z;
	obbWorld.m[3][3] = 1.0f;


	// 逆行列
	Matrix4x4 obbInverse = obbWorld.Inverse();

	// OBBのワールド座標に持ち込む
	Vector3 localOrigin = Transform(ray.start, obbInverse);
	Vector3 localEnd = Transform(ray.start + ray.diff, obbInverse);

	// OBB視点のためAABBを使用する
	Collision::AABB localAABB;
	localAABB.center = Vector3(0.0f, 0.0f, 0.0f);
	localAABB.radius = obb.radius;

	// OBB視点の半直線
	Collision::Ray localRay;
	localRay.start = localOrigin;
	localRay.diff = localEnd - localOrigin;

	// AABBと半直線の当たり判定
	return CollisionCheckFunc(localAABB, localRay);
}

/// @brief 衝突判定
/// @param obb OBB
/// @param segment 線分
/// @return 
bool Engine::CollisionCheckFunc(const Collision::OBB& obb, const Collision::Segment& segment)
{
	// OBBのワールド行列
	Matrix4x4 obbWorld;

	obbWorld.m[0][0] = obb.oriented[0].x;
	obbWorld.m[0][1] = obb.oriented[0].y;
	obbWorld.m[0][2] = obb.oriented[0].z;
	obbWorld.m[0][3] = 0.0f;

	obbWorld.m[1][0] = obb.oriented[1].x;
	obbWorld.m[1][1] = obb.oriented[1].y;
	obbWorld.m[1][2] = obb.oriented[1].z;
	obbWorld.m[1][3] = 0.0f;

	obbWorld.m[2][0] = obb.oriented[2].x;
	obbWorld.m[2][1] = obb.oriented[2].y;
	obbWorld.m[2][2] = obb.oriented[2].z;
	obbWorld.m[2][3] = 0.0f;

	obbWorld.m[3][0] = obb.center.x;
	obbWorld.m[3][1] = obb.center.y;
	obbWorld.m[3][2] = obb.center.z;
	obbWorld.m[3][3] = 1.0f;


	// 逆行列
	Matrix4x4 obbInverse = obbWorld.Inverse();

	// OBBのワールド座標に持ち込む
	Vector3 localOrigin = Transform(segment.start, obbInverse);
	Vector3 localEnd = Transform(segment.start + segment.diff, obbInverse);

	// OBB視点のためAABBを使用する
	Collision::AABB localAABB;
	localAABB.center = Vector3(0.0f, 0.0f, 0.0f);
	localAABB.radius = obb.radius;

	// OBB視点の線分
	Collision::Segment localSegment;
	localSegment.start = localOrigin;
	localSegment.diff = localEnd - localOrigin;

	// AABBと線分の当たり判定
	return CollisionCheckFunc(localAABB, localSegment);
}