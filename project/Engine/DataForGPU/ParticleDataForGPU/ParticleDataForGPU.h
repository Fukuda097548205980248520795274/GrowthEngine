#pragma once
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

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

	/// @brief GPUに送るパーティクルのビューデータ
	struct ParticlePreViewDataForGPU
	{
		// ビュープロジェクション
		Matrix4x4 viewProjection;

		// ビルボード行列
		Matrix4x4 billboard;
	};

	/// @brief GPUに送るパーティクルエミッタのデータ
	struct Particle3DEmitterPointDataForGPU
	{
		// 位置
		Vector3 translate;

		// 放出数
		uint32_t count;

		// 放出間隔の時間
		float frequency;

		// 放出間隔のタイマー
		float frequencyTimer;

		// 放出フラグ
		uint32_t emit;

		float padding[1]; // パディング
	};
}