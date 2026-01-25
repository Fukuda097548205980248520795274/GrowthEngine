#pragma once
#include "Math/Vector/Vector4/Vector4.h"

namespace Engine
{
	// GPUに送るプリミティブモデル用マテリアルデータ
	struct PrimitiveModelMaterialDataForGPU
	{
		// 色
		Vector4 color;
	};
}