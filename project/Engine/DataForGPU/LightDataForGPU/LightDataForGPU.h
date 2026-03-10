#pragma once
#include <cstdint>
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	/// @brief GPUに送るライトデータ
	struct LightDataForGPU
	{
		// 種類
		uint32_t type;

		// 位置
		Vector3 position;

		// ライトが届く距離
		float radius;
	};

	/// @brief GPUに送るライトカリングパラメータ
	struct LightCullingParamForGPU
	{
        // ライト数
        uint32_t numLights;

        // Xタイル数
		uint32_t numTileX;

        // Yタイル数
		uint32_t numTileY;

		float padding[1];

        // 画面サイズ
        Vector2 screenSize;

        // 逆画面サイズ
		Vector2 invScreenSize;

        // 正射影行列
        Matrix4x4 proj;

        // 逆正射影行列
		Matrix4x4 invProj;
	};

	/// @brief GPUに送る平行光源データ
	struct DirectionalLightDataForGPU
	{
		/// @brief 方向
		Vector3 direction;

		/// @brief 輝度
		float intensity;

		/// @brief 色
		Vector4 color;
	};

	/// @brief GPUに送るポイントライトデータ
	struct PointLightDataForGPU
	{
		/// @brief 色
		Vector4 color;

		/// @brief 位置
		Vector3 position;

		/// @brief 輝度
		float intensity;

		/// @brief ライトの届く距離
		float radius;

		/// @brief 減衰率
		float decay;
	};

	/// @brief GPUに送るスポットライトデータ
	struct SpotLightDataForGPU
	{
		/// @brief 色
		Vector4 color;

		/// @brief 位置
		Vector3 position;

		/// @brief 輝度
		float intensity;

		/// @brief 方向
		Vector3 direction;

		/// @brief ライトの届く距離
		float distance;

		/// @brief 減衰率
		float decay;

		/// @brief 光の当たる角度
		float cosAngle;

		/// @brief フォールオフ開始位置
		float cosFalloffStart;
	};

	/// @brief GPUに送るライト数データ
	struct LightNumDataForGPU
	{
		/// @brief 平行光源
		int32_t directionalLight;

		/// @brief ポイントライト
		int32_t pointLight;

		/// @brief スポットライト
		int32_t spotLight;

		float padding0[1];
	};
}