#pragma once
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"
#include "Handle/Handle.h"

namespace Engine
{
	namespace PostEffect
	{
		/// @brief 種類
		enum class Type
		{
			Grayscale,
			Vignetting,
			Smoothing,
			GaussianFilter,
			LuminanceBasedOutline,
			DepthBasedOutline,
			RadialBlur,
			Dissolve,
			WhiteNoise,
			DOF,
			Bloom,
			TAA,
			MotionBlur,
			AfterImage,
			BlurShadow2D,
		};

		/// @brief ラジアルブラー
		struct RadialBlur
		{
			/// @brief 中心位置
			Vector2 center;

			/// @brief サンプル数
			int samples;

			/// @brief ブラー強度
			float power;
		};

		// グレースケール
		struct Grayscale
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
		};

		/// @brief ヴィネット
		struct Vignetting
		{
			// 色
			Vector3 color;

			// ブレンド強度
			float intensity;

			// 減衰カーブ
			float power;
		};

		/// @brief ホワイトノイズ
		struct WhiteNoise
		{
			// 時間
			float time;
		};

		/// @brief 被写界深度
		struct DOF
		{
			// ピントが合う距離
			float focusDistance;

			// ピントが合う範囲
			float focusRange;

			// ブラーの滑らかさ
			float blurFalloff;
		};

		/// @brief ブルーム
		struct Bloom
		{
			// ブルームの強度
			float threshold;

			// ブルームのブレンド強度
			float knee;
		};

		/// @brief TAA
		struct TAA
		{
			// ブレンドファクター
			float blendFactor;

			// ガンマ補正
			float gamma;
		};

		/// @brief モーションブラー
		struct MotionBlur
		{
			uint32_t numSamples; // サンプル数

			float blurScale; // ブラーの強さ
		};

		/// @brief 残像
		struct AfterImage
		{
			float decay; // 残像の減衰率

			float intensity; // 残像の強度

			Vector3 tintColor; // 残像の色
		};

		/// @brief ブラーによる影
		struct BlurShadow2D
		{
			// 影の色
			Vector4 shadowColor;

			// 影のずれ幅
			Vector2 shadowOffset;

			// ぼかしの強さ
			float blurSize;
		};

		/// @brief 輝度ベースのアウトライン
		struct LuminanceBasedOutline
		{
			// スクリーンの解像度
			Vector2 screenResolution;

			// アウトラインの幅
			float outlineWidth;

			// 色の閾値
			float colorThreshold;
		};

		/// @brief 深度ベースのアウトライン
		struct DepthBasedOutline
		{
			// アウトラインの幅
			float outlineWidth;

			// 深度の閾値
			float depthThreshold;

			// 色の閾値
			float colorThreshold;
		};
	}
}