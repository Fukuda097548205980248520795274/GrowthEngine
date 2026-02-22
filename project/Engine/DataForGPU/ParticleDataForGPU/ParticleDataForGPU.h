#pragma once
#include "Math/Vector/Vector3/Vector3.h"
#include "Math/Vector/Vector4/Vector4.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

namespace Engine
{
	struct ParticleCS
	{
		Vector3 translate;

		Vector3 scale;

		float lifeTime;

		Vector3 velocity;

		float currentTime;

		Vector4 color;
	};

	struct PreViewDataForGPU
	{
		Matrix4x4 viewProjection;

		Matrix4x4 billboardMatrix;
	};
}