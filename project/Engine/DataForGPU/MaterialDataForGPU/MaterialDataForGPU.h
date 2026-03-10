#pragma once
#include "Math/Vector/Vector4/Vector4.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	// GPUに送るプリミティブモデル用マテリアルデータ
	struct PrimitiveModelMaterialDataForGPU
	{
        // 色
        Vector4 color;

        // uv行列
        Matrix4x4 uvMatrix;

        // 環境
        float environment;

        // 光沢度
        float shininess;

        // ライティング有効化
        int enableLighting;

        // ディフューズ
        int enableDiffuse;

        // ハーフランバート有効化
        int enableHalfLambert;

        // スペキュラー有効化
        int enableSpecular;

        // ブリンフォン有効化
        int enableBlinnPhong;

        float padding0[1];
	};

	namespace Sprite
	{
		/// @brief GPUに送るマテリアルデータ
		struct MaterialDataForGPU
		{
			// 色
			Vector4 color;

			// UV行列
			Matrix4x4 uvMatrix;
		};
	}
}