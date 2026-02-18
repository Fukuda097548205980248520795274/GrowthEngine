#pragma once
#include <memory>
#include <vector>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>

#include "PostEffectData/PostEffectBaseData.h"

#include "PSO/PSOPostEffect/BasePSOPostEffect/PSORadialBlur/PSORadialBlur.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;
	class OffscreenResource;

	class PostEffectStore
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, IDxcBlob* vertexShaderBlob, Log* log);

		/// @brief 読み込み
		/// @param name 
		/// @param type 
		/// @param device 
		/// @param log 
		PostEffectHandle Load(const std::string& name, PostEffect::Type type, ID3D12Device* device, Log* log);

		/// @brief ポストエフェクトを描画する
		/// @param hPostEffect 
		/// @param commandList 
		/// @param offscreenResource 
		void DrawPostEffect(PostEffectHandle hPostEffect, ID3D12GraphicsCommandList* commandList, OffscreenResource* offscreenResource);


	private:

		// データテーブル
		std::vector<std::unique_ptr<PostEffectBaseData>> dataTable_;


	private:

		// ラジアルブラーPSO
		std::unique_ptr<PSORadialBlur> psoRadialBlur_ = nullptr;
	};
}