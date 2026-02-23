#pragma once
#include "../PrefabPrimitiveBaseData.h"

#include <memory>
#include <list>
#include "Handle/Handle.h"
#include "Resource/StructuredBufferResource/StructuredBufferResource.h"
#include "Data/ModelData/ModelData.h"
#include "DataForGPU/PrefabDataForGPU/PrefabDataForGPU.h"

#include "Application/PrefabInstance/PrefabInstanceStaticModel/PrefabInstanceStaticModel.h"

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

		/// @brief 更新処理
		void Update() override;

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param pso 
		void Register(ID3D12GraphicsCommandList* commandList, BasePSOModel* pso) override;

		/// @brief シャドウマップを描画する
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void DrawShadowMap(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso) override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief インスタンスを生成する
		/// @return 
		void* CreateInstance() override;

		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance() override;

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	private:

		/// @brief パラメータ
		std::unique_ptr<Prefab::StaticModel::Base::Param> param_ = nullptr;

		/// @brief モデルハンドル
		ModelHandle hModel_ = 0;

		/// @brief インスタンスのドローコール
		void DrawCallInstance(const Engine::Prefab::StaticModel::Instance::Param* param);

		/// @brief インスタンステーブル
		std::list<std::unique_ptr<PrefabInstanceStaticModel>> instanceTable_;


	private:

		/// @brief プリミティブリソース
		std::vector<std::unique_ptr<StructuredBufferResource<Prefab::PrimitiveDataForGPU>>> primitiveResource_;

		/// @brief シャドウマップ座標変換用リソース
		std::vector<std::unique_ptr<StructuredBufferResource<Matrix4x4>>> shadowMapTransformationResource_;


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