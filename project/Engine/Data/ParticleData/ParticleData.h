#pragma once
#include <cmath>
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"

namespace Engine
{
	namespace Particle3D
	{
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
		};
	}
}