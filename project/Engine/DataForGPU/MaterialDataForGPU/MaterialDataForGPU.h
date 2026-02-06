#pragma once
#include "Math/Vector/Vector4/Vector4.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	// GPUに送るプリミティブモデル用マテリアルデータ
	struct PrimitiveModelMaterialDataForGPU
	{
		// 色
		Vector4 color;

		// UV行列
		Matrix4x4 uvMatrix;
	};
}