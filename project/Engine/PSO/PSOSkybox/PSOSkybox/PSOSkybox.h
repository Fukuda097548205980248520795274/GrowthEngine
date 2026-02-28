#pragma once
#include "../BasePSOSkybox.h"

namespace Engine
{
	class PSOSkybox : public BasePSOSkybox
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param vertexShaderBlob 
		/// @param pixelShaderBlob 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log) override;
	};
}