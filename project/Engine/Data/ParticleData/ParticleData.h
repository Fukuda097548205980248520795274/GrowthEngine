#pragma once
#include <cmath>
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"

namespace Engine
{
	namespace Particle3D
	{
		// エミッターの図形
		enum class EmitterShape
		{
			Point,
			AABB,
			Sphere
		};

		/// @brief 色
		struct Color
		{
			Vector4 start;

			Vector4 end;
		};

		/// @brief 大きさ
		struct Scale
		{
			float start;

			float end;
		};

		/// @brief 速度
		struct Speed
		{
			float start;

			float end;
		};

		/// @brief 生存時間
		struct LifeTime
		{
			float min;

			float max;
		};

		/// @brief パラメータ
		struct Param
		{
			/// @brief 位置
			Vector3 position;


			// エミッターの図形
			EmitterShape shape;

			/// @brief 放出位置の半径
			float radius1;

			/// @brief 放出位置の半径
			Vector3 radius3;


			/// @brief 色
			Color color;

			/// @brief 大きさ
			Scale scale;

			/// @brief 速度
			Speed speed;

			/// @brief 生存期間
			LifeTime lifeTime;

			/// @brief 放出数
			int32_t count;

			/// @brief 放出間隔
			float frequency;

			/// @brief ビルボード有効化
			bool enableBillboard;


			/// @brief 引力の有無
			bool enableAttract;

			/// @brief 引力の位置
			Vector3 attractPos;

			// 吸引加速度
			float attractAcceleration;
		};
	}
}