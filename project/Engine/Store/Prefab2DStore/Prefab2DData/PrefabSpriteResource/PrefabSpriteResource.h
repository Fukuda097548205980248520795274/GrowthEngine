#pragma once
#include "../Prefab2DBaseData.h"

#include "Data/ModelData/ModelData.h"
#include "Application/PrefabInstance/PrefabInstanceSprite/PrefabInstanceSprite.h"

class PrefabInstanceSprite;

namespace Engine
{
	class PrefabSpriteResource : public Prefab2DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param hPrefabSprite 
		/// @param hTexture 
		/// @param numInstance 
		/// @param name 
		PrefabSpriteResource(Prefab2DHandle hPrefab2D, uint32_t numInstance, const std::string& name, Prefab2DParameter* parameter)
			: Prefab2DBaseData(hPrefab2D, numInstance, name, parameter) {
		}

		/// @brief 初期化
		/// @param vertexResource 
		/// @param indexResource 
		/// @param textureStore 
		/// @param heap 
		/// @param device 
		/// @param log 
		void Initialize(VertexBufferResource<SpriteVertexData>* vertexResource, IndexBufferResource* indexResource,
			TextureStore* textureStore, TextureHandle hTexture, Camera2DStore* cameraStore, DX12Heap* heap, ID3D12Device* device, Log* log);

		/// @brief 更新処理
		void Update() override;

		/// @brief リセット
		void Reset() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param pso 
		void Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso) override;

		/// @brief インスタンスを作成する
		/// @return 
		void* CreateInstance() override;

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	private:

		/// @brief パラメータ
		std::unique_ptr<Prefab2D::Sprite::Base::Param> param_ = nullptr;

		/// @brief テクスチャファイルパス
		std::string textureFilePath_{};

		// プレハブリソース
		std::unique_ptr<StructuredBufferResource<Prefab::SpriteDataForGPU>> resource_ = nullptr;


	private:

		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance() override { instanceTable_.clear(); }

		/// @brief インスタンスのドローコール
		/// @param param 
		void InstanceDrawCall(const Prefab2D::Sprite::Instance::Param* param);

		// インスタンステーブル
		std::list<std::unique_ptr<PrefabInstanceSprite>> instanceTable_;


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