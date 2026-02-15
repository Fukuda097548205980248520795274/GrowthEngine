#pragma once
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"

namespace Engine
{
	/// @brief GPUに送る頂点
	struct VertexDataForGPU
	{
		/// @brief 位置
		Vector4 position;

		/// @brief テクスチャ座標
		Vector2 texcoord;

		/// @brief 法線
		Vector3 normal;
	};
}