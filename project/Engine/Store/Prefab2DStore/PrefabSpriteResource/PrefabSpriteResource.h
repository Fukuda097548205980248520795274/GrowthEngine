#pragma once
#include <memory>
#include "Handle/Handle.h"
#include "Resource/StructuredBufferResource/StructuredBufferResource.h"
#include "Data/Prefab2DData/Prefab2DData.h"
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
	class PrefabSpriteParameter;

	class PrefabSpriteResource
	{
	public:

		/// @brief コンストラクタ
		/// @param hPrefabSprite 
		/// @param hTexture 
		/// @param numInstance 
		/// @param name 
		PrefabSpriteResource(Prefab2DHandle hPrefab2D, uint32_t numInstance, const std::string& name, PrefabSpriteParameter* parameter)
			: hPrefab2D_(hPrefab2D), numInstance_(numInstance), name_(name), parameter_(parameter) {
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
		void Update();

		/// @brief リセット
		void Reset();

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		Prefab2DHandle GetHandle()const { return hPrefab2D_; }

		/// @brief パラメータを取得する
		/// @return 
		Prefab2D::Sprite::Base::Param* GetParam() { return param_.get(); }

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param pso 
		void Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief インスタンスを作成する
		/// @return 
		PrefabInstanceSprite* CreateInstance();

		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance() { instanceTable_.clear(); }

		/// @brief リセット
		void InstanceReset() { useInstance_ = 0; }

		/// @brief デバッグ用パラメータ
		void DebugParameter();


	private:

		// 名前
		std::string name_{};

		/// @brief グループ名
		std::string group_{};

		// ハンドル
		Prefab2DHandle hPrefab2D_ = 0;

		// インスタンス数
		uint32_t numInstance_ = 0;

		/// @brief パラメータ
		std::unique_ptr<Prefab2D::Sprite::Base::Param> param_ = nullptr;

		/// @brief テクスチャファイルパス
		std::string textureFilePath_{};

		// プレハブリソース
		std::unique_ptr<StructuredBufferResource<Prefab::SpriteDataForGPU>> resource_ = nullptr;


	private:

		/// @brief インスタンスのドローコール
		/// @param param 
		void InstanceDrawCall(const Prefab2D::Sprite::Instance::Param* param);

		// 使用インスタンス数
		uint32_t useInstance_ = 0;

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


	private:

		/// @brief パラメータ
		PrefabSpriteParameter* parameter_ = nullptr;
	};
}