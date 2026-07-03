#pragma once
#include "../BasePSOPostEffect.h"

namespace Engine
{
	class PSOBlurShadow2D : public BasePSOPostEffect
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param vertexShaderBlob 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, IDxcBlob* vertexShaderBlob, Log* log) override;
	};
}