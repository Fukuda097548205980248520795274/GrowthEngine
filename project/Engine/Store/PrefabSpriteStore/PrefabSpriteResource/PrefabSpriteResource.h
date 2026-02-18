#pragma once
#include <memory>
#include "Handle/Handle.h"
#include "Resource/StructuredBufferResource/StructuredBufferResource.h"
#include "Data/PrefabData/PrefabData.h"
#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "Data/ModelData/ModelData.h"
#include "DataForGPU/PrefabDataForGPU/PrefabDataForGPU.h"
#include "Application/PrefabInstance/PrefabInstanceSprite/PrefabInstanceSprite.h"

class PrefabInstanceSprite;

namespace Engine
{
	class Log;
	class BasePSOModel;
	class TextureStore;
	class IndexBufferResource;
	class DX12Heap;
	class Camera2DStore;

	class PrefabSpriteResource
	{
	public:

		/// @brief コンストラクタ
		/// @param hPrefabSprite 
		/// @param hTexture 
		/// @param numInstance 
		/// @param name 
		PrefabSpriteResource(PrefabSpriteHandle hPrefabSprite, TextureHandle hTexture, uint32_t numInstance, const std::string& name);

		/// @brief 初期化
		/// @param vertexResource 
		/// @param indexResource 
		/// @param textureStore 
		/// @param heap 
		/// @param device 
		/// @param log 
		void Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource,
			TextureStore* textureStore, Camera2DStore* cameraStore, DX12Heap* heap, ID3D12Device* device, Log* log);

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		PrefabSpriteHandle GetHandle()const { return hPrefabSprite_; }

		/// @brief パラメータを取得する
		/// @return 
		Prefab::Sprite::Base::Param* GetParam() { return param_.get(); }

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param pso 
		void Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief リセット
		void Reset() { useInstance_ = 0; }

		/// @brief ドローコール関数を取得する
		/// @return 
		std::function<void(const Prefab::Sprite::Instance::Param*)> GetDrawCall() { return [this](const Prefab::Sprite::Instance::Param* param) {InstanceDrawCall(param); }; }


	private:

		/// @brief インスタンスのドローコール
		/// @param param 
		void InstanceDrawCall(const Prefab::Sprite::Instance::Param* param);

		// 名前
		std::string name_{};

		// ハンドル
		PrefabSpriteHandle hPrefabSprite_ = 0;

		// インスタンス数
		uint32_t numInstance_ = 0;

		/// @brief パラメータ
		std::unique_ptr<Prefab::Sprite::Base::Param> param_ = nullptr;

		// プレハブリソース
		std::unique_ptr<StructuredBufferResource<Prefab::SpriteDataForGPU>> resource_ = nullptr;


	private:

		/// @brief インスタンステーブル
		std::vector<std::unique_ptr<PrefabInstanceSprite>> instanceTable_;

		// 使用インスタンス数
		uint32_t useInstance_ = 0;


	private:

		// 頂点リソース
		VertexBufferResource<SpriteVertexData>* vertexResource_ = nullptr;

		// インデックスリソース
		IndexBufferResource* indexResource_ = nullptr;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		// カメラストア
		Camera2DStore* cameraStore_ = nullptr;
	};
}