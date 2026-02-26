#pragma once
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"

namespace Engine
{
	namespace Collision3D
	{
		enum class Type
		{
			Sphere,
			AABB,
			OBB,
			Plane,
			Line,
			Ray,
			Segment
		};

		/// @brief 球
		struct Sphere
		{
			/// @brief 中心点
			Vector3 center;

			/// @brief 半径
			float radius;
		};

		/// @brief AABB
		struct AABB
		{
			/// @brief 中心地
			Vector3 center;

			/// @brief 半径
			Vector3 radius;
		};

		/// @brief OBB
		struct OBB
		{
			/// @brief 中心点
			Vector3 center;

			/// @brief 回転軸
			Vector3 oriented[3];

			/// @brief 半径
			Vector3 radius;
		};

		/// @brief 平面
		struct Plane
		{
			/// @brief 法線
			Vector3 normal;

			/// @brief 距離
			float distance;
		};

		/// @brief 直線
		struct Line
		{
			/// @brief 始点
			Vector3 start;

			/// @brief 終点（差分ベクトル）
			Vector3 diff;
		};

		/// @brief 半直線
		struct Ray
		{
			/// @brief 始点
			Vector3 start;

			/// @brief 終点（差分ベクトル）
			Vector3 diff;
		};

		/// @brief 線分
		struct Segment
		{
			/// @brief 始点
			Vector3 start;

			/// @brief 終点（差分ベクトル）
			Vector3 diff;
		};
	}

	namespace Collision2D
	{
		enum class Type
		{
			Circle,
			Sprite
		};

		/// @brief 円
		struct Circle
		{
			/// @brief 中心点
			Vector2 center;

			/// @brief 半径
			float radius;
		};

		/// @brief 矩形
		struct Sprite
		{
			/// @brief 中心点
			Vector2 center;

			/// @brief 半径
			Vector2 radius;
		};
	}
}