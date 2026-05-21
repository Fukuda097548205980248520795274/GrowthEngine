#pragma once
#include "Store/Particle3DStore/Particle3DStore.h"

namespace Engine
{
	class Camera3DStore;

	class DX12Particle
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param heap 
		/// @param modelStore 
		/// @param textureStore
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap, ModelStore* modelStore, TextureStore* textureStore, Log* log);

		/// @brief 3Dパーティクルを読み込む
		/// @param device 
		/// @param commandList 
		/// @param hModel 
		/// @param name 
		/// @param numInstance 
		/// @param log 
		/// @return 
		Particle3DHandle LoadParticle3D(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ModelHandle hModel, const std::string& name,
			uint32_t numInstance,uint32_t emitterNum, Log* log)
		{
			return particle3DStore_->Load(device, commandList, hModel, name, numInstance,emitterNum, log);
		}

		/// @brief 更新処理
		/// @param commandList 
		void Update(ID3D12GraphicsCommandList* commandList);

		/// @brief シーン前のリセット処理
		void PerSceneReset() { particle3DStore_->PerSceneReset(); }


		/// @brief 3Dパーティクル描画処理
		/// @param commandList 
		/// @param handle 
		/// @param cameraStore 
		void Draw3DParticle(ID3D12GraphicsCommandList* commandList, Particle3DHandle handle, const Camera3DStore* cameraStore, OffscreenResource* offscreenResource, DepthResource* depthResource)
		{
			particle3DStore_->Draw(commandList, handle , cameraStore,offscreenResource, depthResource); 
		}

		/// @brief 3Dパーティクル描画処理
		/// @param commandList 
		/// @param name 
		/// @param viewProjection 
		void Draw3DParticle(ID3D12GraphicsCommandList* commandList, const std::string& name, const Camera3DStore* cameraStore,OffscreenResource* offscreenResource, DepthResource* depthResource)
		{ 
			particle3DStore_->Draw(commandList, name, cameraStore, offscreenResource, depthResource);
		}

		/// @brief 3Dパーティクルのパラメータを取得する
		/// @param hParticle 
		Particle3D::Param* Get3DParticleParam(Particle3DHandle hParticle) { return particle3DStore_->GetParam(hParticle); }

		/// @brief 3Dパーティクルのパラメータを取得する
		/// @param name 
		Particle3D::Param* Get3DParticleParam(const std::string& name) { return particle3DStore_->GetParam(name); }

		/// @brief 放出開始
		/// @param handle 
		void Emit3D(Particle3DHandle handle, int32_t emitterIndex) { particle3DStore_->Emit(handle, emitterIndex); }

		/// @brief 放出開始
		/// @param name 
		void Emit3D(const std::string& name, int32_t emitterIndex) { particle3DStore_->Emit(name, emitterIndex); }

		/// @brief 放出停止
		/// @param handle 
		void Stop3D(Particle3DHandle handle, int32_t emitterIndex) { particle3DStore_->Stop(handle, emitterIndex); }

		/// @brief 放出停止
		/// @param name 
		void Stop3D(const std::string& name, int32_t emitterIndex) { particle3DStore_->Stop(name, emitterIndex); }


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter();


	private:

		// 3Dパーティクルストア
		std::unique_ptr<Particle3DStore> particle3DStore_ = nullptr;
	};
}