#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>

#include "PSO/PSOModel/PSOParticle/PSOParticle.h"

namespace Engine
{
	class ShaderCompiler;
	class DX12Heap;
	class ModelStore;
	class TextureStore;
	class Log;

	class Particle3DStore
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param heap 
		/// @param modelStore 
		/// @param textureStore 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap, ModelStore* modelStore, TextureStore* textureStore, Log* log);


	private:

		// 頂点シェーダーのデータ
		Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;

		// ピクセルシェーダーのデータ
		Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;


		/// @brief パーティクルPSO
		std::unique_ptr<PSOParticle> psoParticle_ = nullptr;


	private:

		// DX12ヒープ
		DX12Heap* heap_ = nullptr;

		// モデルストア
		ModelStore* modelStore_ = nullptr;

		// テクスチャストア
		TextureStore* textureStore_ = nullptr;
	};
}