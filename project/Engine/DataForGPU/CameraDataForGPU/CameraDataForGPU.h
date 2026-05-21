#pragma once
#include "Math/Vector/Vector3/Vector3.h"

namespace Engine
{
	struct CameraDataForGPU
	{
		Vector3 position;
		float nearZ;
		float farZ;

		float padding[3]; // パディング
	};
}