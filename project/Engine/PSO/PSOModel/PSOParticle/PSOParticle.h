#pragma once
#include "../BasePSOModel.h"

namespace Engine
{
	class PSOParticle : public BasePSOModel
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param vertexShaderBlob 
		/// @param pixelShaderBlob 
		/// @param log 
		void Initialize(ID3D12Device* device, IDxcBlob* vertexShaderBlob, IDxcBlob* pixelShaderBlob, Log* log) override;

		/// @brief ブレンドモードを初期化する
		void ResetBlendMode() override { blendMode_ = static_cast<int32_t>(BlendMode::kAdd); }
	};
}