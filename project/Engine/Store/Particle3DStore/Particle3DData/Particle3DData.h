#pragma once
#include "DataForGPU/ParticleDataForGPU/ParticleDataForGPU.h"
#include "Handle/Handle.h"
#include "Resource/RWStructuredBufferResource/RWStructuredBufferResource.h"
#include "Resource/ConstantBufferResource/ConstantBufferResource.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

class GrowthEngine;

namespace Engine
{
	class BasePSOModel;
	class BaseComputePSO;
	class ModelStore;
	class TextureStore;

	class Particle3DData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hModel 
		/// @param numInstance 
		Particle3DData(const std::string& name, ModelHandle hModel, uint32_t numInstance) :name_(name), hModel_(hModel), numInstance_(numInstance) {}

		/// @brief 初期化
		/// @param device 
		/// @param commandList 
		/// @param heap 
		/// @param psoDraw 
		/// @param psoInit 
		/// @param log 
		void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap,
			ModelStore* modelStore, TextureStore* textureStore, BasePSOModel* psoDraw, BaseComputePSO* psoInit, Log* log);

		/// @brief 更新処理
		/// @param commandList 
		/// @param psoEmitter 
		/// @param psoUpdate 
		void Update(ID3D12GraphicsCommandList* commandList, BaseComputePSO* psoEmitter, BaseComputePSO* psoUpdate);

		/// @brief 描画処理
		/// @param commandList 
		/// @param psoDraw 
		/// @param viewProjection 
		void Draw(ID3D12GraphicsCommandList* commandList, const Matrix4x4& viewProjection);

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		ModelHandle GetHandle()const { return hModel_; }


	private:

		/// @brief パーティクルリソース
		std::unique_ptr<RWSTructuredBufferResource<Particle3DDataForGPU>> particleResource_ = nullptr;

		/// @brief パーティクル数リソース
		std::unique_ptr<ConstantBufferResource<ParticleNumDataForGPU>> particleNumResource_ = nullptr;

		/// @brief パーティクルビューリソース
		std::unique_ptr<ConstantBufferResource<ParticlePreViewDataForGPU>> particleViewResource_ = nullptr;

		/// @brief エミッターリソース
		std::unique_ptr<ConstantBufferResource<Particle3DEmitterPointDataForGPU>> particleEmitterPointResource_ = nullptr;

		/// @brief パーティクルフレームリソース
		std::unique_ptr<ConstantBufferResource<ParticlePerFrameDataForGPU>> particlePerFrameResource_ = nullptr;

		/// @brief フリーリストインデックスリソース
		std::unique_ptr<RWSTructuredBufferResource<int32_t>> freeListIndexResource_ = nullptr;

		/// @brief フリーリストリソース
		std::unique_ptr<RWSTructuredBufferResource<uint32_t>> freeListResource_ = nullptr;


	private:

		/// @brief 名前
		std::string name_{};

		/// @brief モデルハンドル
		ModelHandle hModel_ = 0;

		/// @brief テクスチャハンドル
		TextureHandle hTexture_ = 0;

		/// @brief インスタンス数
		uint32_t numInstance_ = 0;


	private:

		// エンジン
		GrowthEngine* engine_ = nullptr;

		// モデルストア
		ModelStore* modelStore_ = nullptr;

		// テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief 描画用PSO
		BasePSOModel* psoDraw_ = nullptr;
	};
}