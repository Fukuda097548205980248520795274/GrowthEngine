#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

#include "Handle/Handle.h"
#include "Parameter/Particle3DParameter/Particle3DParameter.h"

#include "Particle3DData/Particle3DData.h"

#include "PSO/PSOModel/PSOParticle/PSOParticle.h"

#include "PSO/ComputePSO/ComputePSOParticle3DInit/ComputePSOParticle3DInit.h"
#include "PSO/ComputePSO/ComputePSOParticle3DEmitterPoint/ComputePSOParticle3DEmitterPoint.h"
#include "PSO/ComputePSO/ComputePSOParticle3DEmitterAABB/ComputePSOParticle3DEmitterAABB.h"
#include "PSO/ComputePSO/ComputePSOParticle3DEmitterSphere/ComputePSOParticle3DEmitterSphere.h"
#include "PSO/ComputePSO/ComputePSOParticle3DUpdateVelocity/ComputePSOParticle3DUpdateVelocity.h"
#include "PSO/ComputePSO/ComputePSOParticle3DUpdateAttract/ComputePSOParticle3DUpdateAttract.h"

namespace Engine
{
	class ShaderCompiler;
	class DX12Heap;
	class ModelStore;
	class TextureStore;
	class Camera3DStore;
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

		/// @brief リセット
		void Reset();

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
		void Draw(ID3D12GraphicsCommandList* commandList, const std::string& name, const Camera3DStore* cameraStore);


	public:

		/// @brief デバッグパラメータ
		void DebugParameter();


	private:

		/// @brief パラメータ
		std::unique_ptr<Particle3DParameter> parameter_ = nullptr;

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


		/// @brief CSパーティクルポイントエミッターPSO
		std::unique_ptr<ComputePSOParticle3DEmitterPoint> computePsoParticle3DEmitterPoint_ = nullptr;

		/// @brief CSパーティクルAABBエミッターPSO
		std::unique_ptr<ComputePSOParticle3DEmitterAABB> computePsoParticle3DEmitterAABB_ = nullptr;

		/// @brief CSパーティクル球エミッターPSO
		std::unique_ptr<ComputePSOParticle3DEmitterSphere> computePsoParticle3DEmitterSphere_ = nullptr;


		/// @brief CSパーティクル速度更新PSO
		std::unique_ptr<ComputePSOParticle3DUpdateVelocity> computePsoParticle3DUpdateVelocity_ = nullptr;

		/// @brief CSパーティクル引力更新PSO
		std::unique_ptr<ComputePSOParticle3DUpdateAttract> computePsoParticle3DUpdateAttract_ = nullptr;


	private:

		// DX12ヒープ
		DX12Heap* heap_ = nullptr;

		// モデルストア
		ModelStore* modelStore_ = nullptr;

		// テクスチャストア
		TextureStore* textureStore_ = nullptr;
	};
}