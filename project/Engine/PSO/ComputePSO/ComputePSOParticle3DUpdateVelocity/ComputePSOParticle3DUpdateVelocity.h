#pragma once
#include "../BaseComputePSO.h"

namespace Engine
{
	/// @brief パーティクル速度更新PSO
	class ComputePSOParticle3DUpdateVelocity : public BaseComputePSO
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log) override;
	};
}