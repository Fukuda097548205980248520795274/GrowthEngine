#pragma once
#include "../Prefab3DBaseData.h"

#include "Data/ModelData/ModelData.h"

#include "Application/PrefabInstance/PrefabInstanceStaticModel/PrefabInstanceStaticModel.h"

namespace Engine
{
	class ModelStore;
	class TextureStore;
	class LightStore;
	class Camera3DStore;
	class Log;
	class SkyboxStore;

	class Prefab3DStaticModelData : public Prefab3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param numInstance 
		/// @param hPrefab 
		/// @param hTexture 
		Prefab3DStaticModelData(const std::string& name, uint32_t numInstance, Prefab3DHandle hPrefab3D, ModelHandle hModel, Prefab3DParameter* parameter);

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

		/// @brief リセット
		void Reset() override;

		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param pso 
		void Register(SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso) override;

		/// @brief シャドウマップを描画する
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void DrawShadowMap(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso) override;

		/// @brief モーションベクターを描画する
		/// @param commandList 
		/// @param pso 
		void RegisterMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso) override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief インスタンスを生成する
		/// @return 
		void* CreateInstance() override;


	public:

		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance() override;

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	private:

		/// @brief パラメータ
		std::unique_ptr<Prefab3D::StaticModel::Base::Param> param_ = nullptr;

		// テクスチャファイルパステーブル
		std::vector<std::string> textureFilePathTable_;

		/// @brief モデルハンドル
		ModelHandle hModel_ = 0;

		/// @brief インスタンスのドローコール
		void DrawCallInstance(const Engine::Prefab3D::StaticModel::Instance::Param* param);

		/// @brief インスタンステーブル
		std::list<std::unique_ptr<PrefabInstanceStaticModel>> instanceTable_;


	private:

		/// @brief プリミティブリソース
		std::vector<std::unique_ptr<StructuredBufferResource<Prefab::PrimitiveDataForGPU>>> primitiveResource_;

		/// @brief シャドウマップ座標変換用リソース
		std::vector<std::unique_ptr<StructuredBufferResource<Matrix4x4>>> shadowMapTransformationResource_;

		/// @brief モーションベクターリソース
		std::vector<std::unique_ptr<StructuredBufferResource<MotionVectorDataForGPU>>> motionVectorResources_;


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