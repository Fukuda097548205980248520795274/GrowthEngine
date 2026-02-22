#pragma once
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	struct ParticleCS
	{
		Vector3 translate;

		Vector3 scale;

		float lifeTime;

		Vector3 velocity;

		float currentTime;

		Vector4 color;
	};

	struct PreViewDataForGPU
	{
		Matrix4x4 viewProjection;

		Matrix4x4 billboardMatrix;
	};

	struct EmitterSphere
	{
		/// @brief 位置
		Vector3 translate;

		/// @brief 半径
		float radius;

		/// @brief 放出数
		uint32_t count;

		/// @brief 射出間隔
		float frequency;

		/// @brief 射出間隔調整用時間
		float frequencyTime;

		/// @brief 射出許可
		uint32_t emit;
	};
}