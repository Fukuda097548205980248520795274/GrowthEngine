#pragma once
#include "Math/Vector/Vector2/Vector2.h"

namespace Engine
{
	namespace PostEffect
	{
		/// @brief 種類
		enum class Type
		{
			RadialBlur
		};

		/// @brief ラジアルブラー
		struct RadialBlur
		{
			Vector2 center;
			float blur;
			int sampleCount;

			// --- 色補正パラメータ ---
			float saturation; // 彩度 (1.0 = 元のまま)
			float contrast; // コントラスト (1.0 = 元のまま)
			float brightness; // 明るさ (0 = 変化なし)
		};
	}
}