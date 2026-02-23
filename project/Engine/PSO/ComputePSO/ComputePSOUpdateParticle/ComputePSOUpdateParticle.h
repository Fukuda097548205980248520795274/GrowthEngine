#pragma once
#include "../BaseComputePSO.h"

namespace Engine
{
	class ComputePSOUpdateParticle : public BaseComputePSO
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log) override;
	};
}