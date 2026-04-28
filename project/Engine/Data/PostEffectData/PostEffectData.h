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
			TAA
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

		/// @brief 輝度ベースの輪郭抽出
		struct LuminanceBasedOutline
		{
			// RGBチャンネルの重み
			Vector3 colorWeight;

			// 輝度の閾値
			float intensity;
		};

		/// @brief ディゾルブ
		struct Dissolve
		{
			// 閾値
			float threshold;

			// マスクテクスチャ
			TextureHandle hMaskTexture;

			// エッジの幅
			float edgeDiff;

			// エッジの色
			Vector3 edgeColor;
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
	}
}