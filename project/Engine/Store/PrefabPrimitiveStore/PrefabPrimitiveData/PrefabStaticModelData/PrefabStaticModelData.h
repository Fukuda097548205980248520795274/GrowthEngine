#pragma once
#include "../PrefabPrimitiveBaseData.h"

#include <memory>
#include "Handle/Handle.h"
#include "Resource/StructuredBufferResource/StructuredBufferResource.h"
#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "Data/ModelData/ModelData.h"
#include "DataForGPU/PrefabDataForGPU/PrefabDataForGPU.h"

namespace Engine
{
	class ModelStore;
	class TextureStore;
	class LightStore;
	class Camera3DStore;
	class Log;

	class PrefabStaticModelData : public PrefabPrimitiveBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param numInstance 
		/// @param hPrefab 
		/// @param hTexture 
		PrefabStaticModelData(const std::string& name, uint32_t numInstance, PrefabPrimitiveHandle hPrefab, ModelHandle hModel);

		/// @brief 初期化
		/// @param modelStore 
		/// @param textureStore 
		/// @param lightStore 
		/// @param cameraStore 
		/// @param heap 
		/// @param device 
		/// @param log 
		void Initialize(ModelStore* modelStore, TextureStore* textureStore, LightStore* lightStore, Camera3DStore* cameraStore,
			DX12Heap* heap, ID3D12Device* device, Log* log);


	private:

		/// @brief パラメータ
		std::unique_ptr<Prefab::StaticModel::Param> param_ = nullptr;

		/// @brief モデルハンドル
		ModelHandle hModel_ = 0;


	private:


		/// @brief モデルストア
		ModelStore* modelStore_ = nullptr;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;

		/// @brief カメラストア
		Camera3DStore* cameraStore_ = nullptr;
	};
}