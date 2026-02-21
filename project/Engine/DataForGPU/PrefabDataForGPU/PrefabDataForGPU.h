#pragma once
#include "Math/Vector/Vector4/Vector4.h"
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	namespace Prefab
	{
		struct SpriteDataForGPU
		{
            // WVP行列
            Matrix4x4 worldViewProjection;

            // 色
            Vector4 color;

            // UVトランスフォーム
            Matrix4x4 uvTransform;

			// アンカーポイント
			Vector2 anchor;
		};

		struct PrimitiveDataForGPU
		{
			// WVP行列
			Matrix4x4 worldViewProjection;

			// ワールド行列
			Matrix4x4 world;

			// ワールド逆転置行列
			Matrix4x4 worldInverseTranspose;

			// 色
			Vector4 color;

			// UVトランスフォーム
			Matrix4x4 uvTransform;
		};
	}
}