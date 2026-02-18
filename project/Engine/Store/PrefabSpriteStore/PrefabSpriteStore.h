#pragma once
#include "PrefabSpriteResource/PrefabSpriteResource.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include <vector>

namespace Engine
{
	class PrefabSpriteStore
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log);

		/// @brief 更新処理
		void Update();

		/// @brief 読み込み
		/// @param name 
		/// @param hTexture 
		/// @param numInstance 
		/// @param textureStore 
		/// @param device 
		/// @param log 
		/// @return 
		PrefabSpriteHandle Load(const std::string& name, TextureHandle hTexture, uint32_t numInstance,
			TextureStore* textureStore,Camera2DStore* cameraStore, DX12Heap* heap, ID3D12Device* device, Log* log);

		/// @brief コマンドリストに登録する
		/// @param hSprite 
		/// @param commandList 
		/// @param pso 
		void Register(PrefabSpriteHandle hPrefabSprite,ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief ドローコール関数を取得する
		/// @return 
		std::function<void(const Prefab::Sprite::Instance::Param*)> GetDrawCall(PrefabSpriteHandle hPrefabSprite) { return dataTable_[hPrefabSprite]->GetDrawCall(); }

		/// @brief パラメータを取得する
		/// @return 
		Prefab::Sprite::Base::Param* GetParam(PrefabSpriteHandle hPrefabSprite) { return dataTable_[hPrefabSprite]->GetParam(); }

		/// @brief リセット
		void Reset();


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<PrefabSpriteResource>> dataTable_;


	private:

		// 頂点リソース
		std::unique_ptr<VertexBufferResource<SpriteVertexData>> vertexResource_ = nullptr;

		/// @brief インデックスリソース
		std::unique_ptr<IndexBufferResource> indexResource_ = nullptr;
	};
}