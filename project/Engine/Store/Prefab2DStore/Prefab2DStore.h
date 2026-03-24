#pragma once
#include "PrefabSpriteResource/PrefabSpriteResource.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"
#include <vector>

#include "Parameter/Prefab2DParameter/Prefab2DParameter.h"

namespace Engine
{
	class Prefab2DStore
	{
	public:

		/// @brief コンストラクタ
		Prefab2DStore();

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
		Prefab2DHandle Load(const std::string& name, TextureHandle hTexture, uint32_t numInstance,
			TextureStore* textureStore,Camera2DStore* cameraStore, DX12Heap* heap, ID3D12Device* device, Log* log);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param pso 
		void Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief パラメータを取得する
		/// @return 
		Prefab2D::Sprite::Base::Param* GetParam(Prefab2DHandle hPrefab2D) { return dataTable_[hPrefab2D]->GetParam(); }

		/// @brief パラメータを取得する
		/// @param name 
		/// @return 
		Prefab2D::Sprite::Base::Param* GetParam(const std::string& name) { return dataTable_[nameTable_[name]]->GetParam(); }

		/// @brief インスタンスを作成する
		/// @param hPrefabSprite 
		/// @return 
		PrefabInstanceSprite* CreateInstance(Prefab2DHandle hPrefab2D) { return dataTable_[hPrefab2D]->CreateInstance(); }

		/// @brief インスタンスを作成する
		/// @param name 
		/// @return 
		PrefabInstanceSprite* CreateInstance(const std::string& name) { return dataTable_[nameTable_[name]]->CreateInstance(); }

		/// @brief リセット
		void Reset();

		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance();

		/// @brief デバッグ用パラメータ
		void DebugParameter();


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<PrefabSpriteResource>> dataTable_;

		/// @brief 名前テーブル
		std::unordered_map<std::string, Prefab2DHandle> nameTable_;

		// パラメータ
		std::unique_ptr<Prefab2DParameter> parameter_ = nullptr;


	private:

		// 頂点リソース
		std::unique_ptr<VertexBufferResource<SpriteVertexData>> vertexResource_ = nullptr;

		/// @brief インデックスリソース
		std::unique_ptr<IndexBufferResource> indexResource_ = nullptr;
	};
}