#pragma once
#define NOMINMAX
#include "DataForGPU/ParticleDataForGPU/ParticleDataForGPU.h"
#include "Data/ParticleData/ParticleData.h"
#include "Handle/Handle.h"
#include "Resource/RWStructuredBufferResource/RWStructuredBufferResource.h"
#include "Resource/ConstantBufferResource/ConstantBufferResource.h"
#include "Resource/StructuredBufferResource/StructuredBufferResource.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"

class GrowthEngine;

namespace Engine
{
	class BasePSOModel;
	class BaseComputePSO;
	class ModelStore;
	class TextureStore;
	class Particle3DParameter;
	class Camera3DStore;

	class Particle3DData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hModel 
		/// @param numInstance 
		Particle3DData(const std::string& name, ModelHandle hModel, uint32_t numInstance, uint32_t emitterNum) :
			name_(name), hModel_(hModel), numInstance_(numInstance), emitterNum_(emitterNum) {
			isLoad_ = true;
		}

		/// @brief 初期化
		/// @param device 
		/// @param commandList 
		/// @param heap 
		/// @param psoDraw 
		/// @param psoInit 
		/// @param log 
		void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, Particle3DParameter* parameter,
			ModelStore* modelStore, TextureStore* textureStore, BasePSOModel* psoDraw, BaseComputePSO* psoInit, Log* log);

		/// @brief リセット
		void Reset();

		/// @brief シーン前のリセット処理
		void PerSceneReset();

		/// @brief 更新処理
		/// @param commandList 
		/// @param psoEmitter 
		/// @param psoUpdate 
		void Update(ID3D12GraphicsCommandList* commandList, BaseComputePSO* psoEmitter, BaseComputePSO* psoUpdate);

		/// @brief 放出開始
		/// @param emitterIndex 
		void Emit(int32_t emitterIndex) { param_->emitter[emitterIndex].isStart = true; }

		/// @brief 放出停止
		/// @param emitterIndex 
		void Stop(int32_t emitterIndex) { param_->emitter[emitterIndex].isStart = false; }

		/// @brief 描画処理
		/// @param commandList 
		/// @param psoDraw 
		/// @param viewProjection 
		void Draw(ID3D12GraphicsCommandList* commandList, const Camera3DStore* cameraStore);

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		ModelHandle GetHandle()const { return param_->hModel; }

		/// @brief パラメータを取得する
		/// @return 
		Particle3D::Param* GetParam()const { return param_.get(); }

		/// @brief 引力を有効にしているかどうか
		/// @return 
		bool EnableAttract()const { return param_->attract.enableAttract; }

		/// @brief エミッターの形状IDを取得する
		/// @return 
		Particle3D::EmitterShape GetShape()const { return param_->shape; }


	public:

		/// @brief デバッグパラメータ
		void DebugParameter();


	private:

		/// @brief パーティクルリソース
		std::unique_ptr<RWSTructuredBufferResource<Particle3DDataForGPU>> particleResource_ = nullptr;

		/// @brief パーティクル数リソース
		std::unique_ptr<ConstantBufferResource<ParticleNumDataForGPU>> particleNumResource_ = nullptr;

		/// @brief エミッターリソース
		std::unique_ptr<StructuredBufferResource<Particle3DEmitterDataForGPU>> emitterResource_ = nullptr;

		/// @brief パーティクルビューリソース
		std::unique_ptr<ConstantBufferResource<ParticlePreViewDataForGPU>> particleViewResource_ = nullptr;

		/// @brief 放出設定リソース
		std::unique_ptr<ConstantBufferResource<Particle3DEmitOptionDataForGPU>> emitOptionResource_ = nullptr;

		/// @brief エミッター形状リソース
		std::unique_ptr<ConstantBufferResource<Particle3DEmitterShapeDataForGPU>> particleEmitterShapeResource_ = nullptr;

		/// @brief 引力リソース
		std::unique_ptr<ConstantBufferResource<Particle3DAttractDataForGPU>> particleAttractResource_ = nullptr;

		/// @brief パーティクルフレームリソース
		std::unique_ptr<ConstantBufferResource<ParticlePerFrameDataForGPU>> particlePerFrameResource_ = nullptr;

		/// @brief フリーリストインデックスリソース
		std::unique_ptr<RWSTructuredBufferResource<int32_t>> freeListIndexResource_ = nullptr;

		/// @brief フリーリストリソース
		std::unique_ptr<RWSTructuredBufferResource<uint32_t>> freeListResource_ = nullptr;


	private:

		/// @brief 名前
		std::string name_{};

		/// @brief グループ
		std::string group_{};

		/// @brief インスタンス数
		uint32_t numInstance_ = 0;

		// エミッター数
		uint32_t emitterNum_ = 0;

		/// @brief モデルハンドル
		ModelHandle hModel_ = 0;

		/// @brief テクスチャハンドル
		TextureHandle hTexture_ = 0;

		/// @brief テクスチャのファイルパス
		std::string textureFilePath_{};

		/// @brief ロードフラグ
		bool isLoad_ = false;


		/// @brief パラメータ
		std::unique_ptr<Particle3D::Param> param_ = nullptr;


	private:

		// エンジン
		GrowthEngine* engine_ = nullptr;

		// パラメータ
		Particle3DParameter* parameter_ = nullptr;

		// モデルストア
		ModelStore* modelStore_ = nullptr;

		// テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief 描画用PSO
		BasePSOModel* psoDraw_ = nullptr;
	};
}