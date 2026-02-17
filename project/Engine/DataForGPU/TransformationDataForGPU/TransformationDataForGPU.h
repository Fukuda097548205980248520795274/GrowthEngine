#pragma once
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	struct PrimitiveModelTransformationDataForGPU
	{
		// ワールドビュー正射影行列
		Matrix4x4 worldViewProjectionMatrix;

		// ワールド行列
		Matrix4x4 worldMatrix;

		// ワールド逆転置行列
		Matrix4x4 worldInverseTransposeMatrix;
	};

	namespace Sprite
	{
		/// @brief 座標変換
		struct TransformationDataForGPU
		{
			/// @brief ビュープロジェクション行列
			Matrix4x4 worldViewProjectionMatrix;

			/// @brief アンカー
			Vector2 anchor;
		};
	}
}