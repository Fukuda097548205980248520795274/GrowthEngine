#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "Handle/Handle.h"

#include "Particle3DData/Particle3DData.h"

#include "PSO/PSOModel/PSOParticle/PSOParticle.h"

#include "PSO/ComputePSO/ComputePSOParticle3DInit/ComputePSOParticle3DInit.h"
#include "PSO/ComputePSO/ComputePSOParticle3DEmitterPoint/ComputePSOParticle3DEmitterPoint.h"

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

		/// @brief 読み込む
		/// @param device 
		/// @param commandList 
		/// @param hModel 
		/// @param name 
		/// @param numInstance 
		/// @param log 
		Particle3DHandle Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,ModelHandle hModel, const std::string& name, uint32_t numInstance, Log* log);

		/// @brief 更新処理
		/// @param commandList 
		void Update(ID3D12GraphicsCommandList* commandList);

		/// @brief 描画処理
		/// @param commandList 
		/// @param name 
		/// @param viewProjection 
		void Draw(ID3D12GraphicsCommandList* commandList, const std::string& name, const Matrix4x4& viewProjection);


	public:

		/// @brief データテーブル
		std::vector<std::unique_ptr<Particle3DData>> dataTable_{};

		/// @brief 名前テーブル
		std::unordered_map<std::string, ModelHandle> nameTable_{};


	private:

		// 頂点シェーダーのデータ
		Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;

		// ピクセルシェーダーのデータ
		Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;


	private:

		/// @brief パーティクルPSO
		std::unique_ptr<PSOParticle> psoParticle_ = nullptr;

		/// @brief CSパーティクル初期化PSO
		std::unique_ptr<ComputePSOParticle3DInit> computePsoParticle3DInit_ = nullptr;

		/// @brief CSパーティクルエミッターポイントPSO
		std::unique_ptr<ComputePSOParticle3DEmitterPoint> computePsoParticle3DEmitterPoint_ = nullptr;


	private:

		// DX12ヒープ
		DX12Heap* heap_ = nullptr;

		// モデルストア
		ModelStore* modelStore_ = nullptr;

		// テクスチャストア
		TextureStore* textureStore_ = nullptr;
	};
}