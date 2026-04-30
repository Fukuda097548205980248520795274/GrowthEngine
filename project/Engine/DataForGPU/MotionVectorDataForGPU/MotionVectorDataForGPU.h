#pragma once
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	struct MotionVectorDataForGPU
	{
		// 前フレームのワールドビュー射影行列
		Matrix4x4 prevWVPMatrix;

		// 現フレームのワールドビュー射影行列
		Matrix4x4 currentWVPMatrix;

		// 残像用マスク
		float afterImageMask;

		// モーションブラー用マスク
		float motionBlurMask;

		Vector2 padding; // パディング
	};
}