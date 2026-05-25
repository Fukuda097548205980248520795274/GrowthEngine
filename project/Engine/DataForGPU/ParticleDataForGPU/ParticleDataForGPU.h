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

		/// @brief 初期の色
		Vector4 startColor;

		/// @brief 最後の色
		Vector4 endColor;

		/// @brief 方向
		Vector3 direction;

		// 初期の速度
		float startSpeed;

		/// @brief 放出位置
		Vector3 emitPos;

		// 最後の速度
		float endSpeed;

		// 回転
		Quaternion rotation;

		// 初期の回転
		Quaternion startRotation;

		// 最後の回転
		Quaternion endRotation;

		// 初期の大きさ
		Vector3 startScale;

		// 最後の大きさ
		Vector3 endScale;
	};

	/// @brief GPUに送るパーティクルの数のデータ
	struct ParticleNumDataForGPU
	{
		// パーティクル数
		int particleNum;

		// エミッター数
		int emitterNum;

		float padding[2];
	};

	/// @brief GPUに送るパーティクルのビューデータ
	struct ParticlePreViewDataForGPU
	{
		// ビュープロジェクション
		Matrix4x4 viewProjection;

		// ビルボード行列
		Matrix4x4 billboard;
	};

	/// @brief GPUに送る放出設定のデータ
	struct Particle3DEmitOptionDataForGPU
	{
		/// @brief 初期の色
		Vector4 startColor;

		/// @brief 最後の色
		Vector4 endColor;

		// 初期の大きさ
		Vector3 startScale;

		// 最小の生存時間
		float minLifeTime;

		// 最後の大きさ
		Vector3 endScale;

		// 最大の生存時間
		float maxLifeTime;

		// 初期の速度
		float startSpeed;

		// 最後の速度
		float endSpeed;

		float padding2[2]; // パディング

		// 初期の回転
		Quaternion startRotation;

		// 最後の回転
		Quaternion endRotation;
	};

	// @brief GPUに送るエミッターのデータ
	struct Particle3DEmitterDataForGPU
	{
		// 放出位置
		Vector4 position;

		// 放出数
		uint32_t count;

		// 放出間隔の時間
		float frequency;

		// 放出間隔のタイマー
		float frequencyTimer;

		// 放出フラグ
		uint32_t emit;
	};

	// @brief GPUに送るパーティクルの引き寄せのデータ
	struct Particle3DAttractDataForGPU
	{
		// 引き寄せる位置
		Vector3 position;

		// 吸引加速度
		float acceleration;
	};

	// @brief GPUに送るパーティクルの放出形状のデータ
	struct Particle3DEmitterShapeDataForGPU
	{
		/// @brief 半径
		float radius1;

		/// @brief 半径
		Vector3 radius3;
	};

	/// @brief GPUに送るパーティクルのフレームごとのデータ
	struct ParticlePerFrameDataForGPU
	{
		// デルタタイム
		float deltaTime;

		// 経過時間
		float time;

		float padding[2]; // パディング
	};

	// @brief GPUに送るパーティクルの有効フラグのデータ
	struct ParticleEnableDataForGPU
	{
		// パーティクル有効フラグ
		int32_t softParticle;

		float padding[3]; // パディング
	};
}