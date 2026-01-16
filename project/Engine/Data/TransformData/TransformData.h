#pragma once
#include "Math/Vector/Vector2/Vector2.h"
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Quaternion/Quaternion.h"

namespace Engine
{
	// 2Dトランスフォーム
	struct Transform2D
	{
		// 拡縮
		Vector2 scale;

		// 回転
		float rotation;

		// 移動
		Vector2 translation;
	};

	// オイラー角の3Dトランスフォーム
	struct EulerTransform3D
	{
		// 拡縮
		Vector3 scale;

		// 回転
		Vector3 rotate;

		// 移動
		Vector3 translate;
	};

	// クォータニオンの3Dトランスフォーム
	struct QuaternionTransform3D
	{
		// 拡縮
		Vector3 scale;

		// 回転
		Quaternion rotate;

		// 移動
		Vector3 translate;
	};
}