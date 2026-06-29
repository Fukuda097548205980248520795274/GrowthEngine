#pragma once
#include "../BasePSOModel.h"

namespace Engine
{
	class PSOFullscreen : public BasePSOModel
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param vertexShaderBlob 
		/// @param pixelShaderBlob 
		/// @param log 
		void Initialize(ID3D12Device* device, IDxcBlob* vertexShaderBlob, IDxcBlob* pixelShaderBlob, Log* log) override;
	};
}