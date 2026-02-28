#pragma once
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"
#include "Math/Vector/Vector4/Vector4.h"
#include "Math/Vector/Vector3/Vector3.h"

namespace Engine
{
	namespace SkyboxData
	{
		/// @brief マテリアル
		struct Material
		{
			/// @brief 色
			Vector4 color;
		};

		/// @brief トランスフォーム
		struct Transform
		{
			/// @brief 拡縮
			Vector3 scale;

			/// @brief 回転
			Vector3 rotate;

			/// @brief 移動
			Vector3 translate;
		};

		/// @brief パラメータ
		struct Param
		{
			/// @brief トランスフォーム
			Transform transform;

			/// @brief マテリアル
			Material material;
		};
	}
}