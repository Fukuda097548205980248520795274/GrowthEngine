#pragma once
#include "Math/Vector/Vector3/Vector3.h"

namespace Engine
{
	/// @brief 平行光源パラメータ
	struct DirectionalLightParam
	{
		// 位置
		Vector3 position;

		// 向き
		Vector3 direction;

		// 輝度
		float intensity;

		// 色
		Vector3 color;
	};

	/// @brief ポイントライトパラメータ
	struct PointLightParam
	{
		// 位置
		Vector3 position;

		// 輝度
		float intensity;

		// 色
		Vector3 color;

		// ライトの届く距離
		float radius;

		// 減衰率
		float decay;
	};

	/// @brief スポットライトパラメータ
	struct SpotLightParam
	{
		// 位置
		Vector3 position;

		// 輝度
		float intensity;

		// 色
		Vector3 color;

		// 向き
		Vector3 direction;

		// ライトの届く距離
		float distance;

		// 減衰率
		float decay;

		// 光の当たる角度
		float cosAngle;

		// フォールオフ開始角度
		float cosFalloffStart;
	};
}