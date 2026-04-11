#pragma once
#include "Store/Particle3DStore/Particle3DStore.h"

namespace Engine
{
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

		/// @brief リセット
		void Reset();

		/// @brief 3Dパーティクルを読み込む
		/// @param device 
		/// @param commandList 
		/// @param hModel 
		/// @param name 
		/// @param numInstance 
		/// @param log 
		/// @return 
		Particle3DHandle LoadParticle3D(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ModelHandle hModel, const std::string& name, uint32_t numInstance, Log* log)
		{
			return particle3DStore_->Load(device, commandList, hModel, name, numInstance, log);
		}

		/// @brief 更新処理
		/// @param commandList 
		void Update(ID3D12GraphicsCommandList* commandList);

		/// @brief 描画処理
		/// @param commandList 
		/// @param name 
		/// @param viewProjection 
		void Draw3DParticle(ID3D12GraphicsCommandList* commandList, const std::string& name, const Matrix4x4& viewProjection) { particle3DStore_->Draw(commandList, name, viewProjection); }


	private:

		// 3Dパーティクルストア
		std::unique_ptr<Particle3DStore> particle3DStore_ = nullptr;
	};
}