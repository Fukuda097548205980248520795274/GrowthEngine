#pragma once
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"

namespace Engine
{
	/// @brief GPUに送るパーティクルデータ
	struct Particle3DDataForGPU
	{
		/// @brief 位置
		Vector3 translate;

		/// @brief 生存時間
		float lifeTime;

		/// @brief 大きさ
		Vector3 scale;

		/// @brief 現在の時間
		float currentTime;

		/// @brief 色
		Vector4 color;

		/// @brief 速度
		Vector3 velocity;
	};

	/// @brief GPUに送るパーティクルの数のデータ
	struct ParticleNumDataForGPU
	{
		// 数
		int num;

		float padding[3];
	};
}