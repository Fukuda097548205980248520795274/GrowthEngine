#pragma once
#include "../BasePSOShadowMap.h"

namespace Engine
{
	class PSOShadowMap : public BasePSOShadowMap
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log) override;
	};
}