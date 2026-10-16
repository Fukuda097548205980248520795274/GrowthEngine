#pragma once
#include "../BasePSOPostEffect.h"

namespace Engine
{
	/// @brief ラジアルブラーPSO
	class PSORadialBlur : public BasePSOPostEffect
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