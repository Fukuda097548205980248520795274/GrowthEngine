#pragma once
#include "Math/Vector/Vector3/Vector3.h"

namespace Engine
{
	/// @brief 平行光源パラメータ
	struct DirectionalLightParam
	{
		// 向き
		Vector3 direction;

		// 輝度
		float intensity;

		// 色
		Vector3 color;
	};
}