#pragma once
#include "Math/Vector/Vector3/Vector3.h"

namespace Engine
{
	/// @brief GPUに送るカメラデータ
	struct CameraDataForGPU
	{
		/// @brief 位置
		Vector3 position;

		/// @brief 近平面の距離
		float nearZ;

		/// @brief 遠平面の距離
		float farZ;

		float padding[3]; // パディング
	};
}