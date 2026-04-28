#pragma once
#include "../BasePSOMotionVector.h"

namespace Engine
{
	class PSOMotionVectorPrefab : public BasePSOMotionVector
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param pixelShaderBlob 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, IDxcBlob* pixelShaderBlob, Log* log) override;
	};
}