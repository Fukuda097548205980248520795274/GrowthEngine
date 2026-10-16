#pragma once
#include "../BaseComputePSO.h"

namespace Engine
{
	/// @brief UV球PSO
	class ComputePSOUVSphere : public BaseComputePSO
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log) override;
	};
}