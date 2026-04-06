#pragma once
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"

namespace Engine
{
	namespace PostEffect
	{
		/// @brief GPUに送るラジアルブラー
		struct RadialBlurDataForGPU
		{
            Vector2 resolution;
            Vector2 center;
            float blur;
            int sampleCount;

            // --- 色補正パラメータ ---
            float saturation; // 彩度 (1.0 = 元のまま)
            float contrast; // コントラスト (1.0 = 元のまま)
            float brightness; // 明るさ (0 = 変化なし)
		};


		/// @brief GPUに送るグレースケール
        struct GrayscaleDataForGPU
        {
            // RGBチャンネルの重み
            Vector3 colorWeight;
            // エフェクトのブレンド強度
            float intensity;
            // ティントカラー
            Vector3 tint;
            // コントラスト
            float contrast;
            // 明るさ
            float brightness;

			Vector3 padding0[3]; // 16バイトアラインメントのためのパディング
        };
	}
}