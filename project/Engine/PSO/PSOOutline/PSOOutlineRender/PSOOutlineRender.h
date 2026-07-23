#pragma once
#include "../BasePSOOutline.h"

namespace Engine
{
	class PSOOutlineRender : public BasePSOOutline
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log) override;
	};
}