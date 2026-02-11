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
}