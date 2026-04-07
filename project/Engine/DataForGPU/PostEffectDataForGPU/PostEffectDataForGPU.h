#pragma once
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	namespace PostEffect
	{
		/// @brief GPUに送るラジアルブラー
		struct RadialBlurDataForGPU
		{
            /// @brief 中心位置
            Vector2 center;

            /// @brief サンプル数
            int samples;

            /// @brief ブラー強度
            float power;
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

		/// @brief GPUに送るヴィネット
        struct VignettingDataForGPU
        {
            // 色
            Vector3 color;

            // ブレンド強度
            float intensity;

            // 減衰カーブ
            float power;

			Vector3 padding0[3]; // 16バイトアラインメントのためのパディング
        };

		/// @brief GPUに送る輝度ベースの輪郭抽出
        struct LuminanceBasedOutlineDataForGPU
        {
            // RGBチャンネルの重み
            Vector3 colorWeight;

            // 輝度の閾値
            float intensity;
        };

		/// @brief GPUに送る深度ベースの輪郭抽出
        struct DepthBasedOutlineDataForGPU
        {
            Matrix4x4 projectionInverse;
        };
	}
}