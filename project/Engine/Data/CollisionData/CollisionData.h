#pragma once
#include "Math/Vector/Vector3/Vector3.h"

namespace Engine
{
	namespace Collision
	{
		enum class Type
		{
			Sphere,
			AABB,
			OBB
		};

		/// @brief 球
		struct Sphere
		{
			/// @brief 中心地
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
	}
}