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

			float padding0[3]; // 16バイトアラインメントのためのパディング
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

			float padding0[3]; // 16バイトアラインメントのためのパディング
        };

        /// @brief GPUに送るホワイトノイズ
        struct WhiteNoiseDataForGPU
        {
            // 時間
            float time;

            float padding0[3]; // 16バイトアラインメントのためのパディング
        };

        /// @brief GPUに送る被写界深度
        struct DOFDataForGPU
        {
            // ピントが合う距離
            float focusDistance;

            // ピントが合う範囲
            float focusRange;

            // ブラーの滑らかさ
            float blurFalloff;

            // 近クリップ面
            float zNear;


            // 遠クリップ面
            float zFar;

			float padding0[3]; // 16バイトアラインメントのためのパディング
        };

		/// @brief GPUに送る高輝度抽出
        struct HighLuminanceExtractionDataForGPU
        {
            // 輝度の閾値
            float threshold;

			// 滑らかさ
            float knee;

			float padding[2]; // 16バイトアラインメントのためのパディング
        };

        /// @brief GPUに送るTAA
        struct TAADataForGPU
        {
			// ブレンドファクター
			float blendFactor;

			// ガンマ補正値
            float gamma;

			float padding[2]; // 16バイトアラインメントのためのパディング
        };

		/// @brief GPUに送るモーションブラー
        struct MotionBlurDataForGPU
        {
            // ブラー強度
			uint32_t numSamples;

			// ブラーのスケール
            float blurScale;

			float padding[2]; // 16バイトアラインメントのためのパディング
        };

		/// @brief GPUに送る残像
		struct AfterImageDataForGPU
        {
            // 残像の減衰率
			float decay;

            // 残像の強度
			float intensity;

			float padding0[2]; // 16バイトアラインメントのためのパディング

            // 残像の色
			Vector3 color;

			float padding1[1]; // 16バイトアラインメントのためのパディング

            // 現在のフレームのビュー射影行列の逆行列
			Matrix4x4 InvCurrentViewProjection;

            // 前のフレームのビュー射影行列
			Matrix4x4 PrevViewProjection;
        };

        /// @brief ブラーによる影
        struct BlurShadow2DDataForGPU
        {
            // 影の色
            Vector4 shadowColor;

            // 影のずれ幅
            Vector2 shadowOffset;

            // ぼかしの強さ
            float blurSize;

			float padding[1]; // 16バイトアラインメントのためのパディング
        };
	}
}