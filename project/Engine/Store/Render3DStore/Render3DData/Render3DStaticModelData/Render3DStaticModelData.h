#pragma once
#include "../Render3DBaseData.h"

namespace Engine
{
	class ModelStore;
	class BasePSOModel;
	class BasePSOShadowMap;
	class Log;
	class TextureStore;
	class LightStore;
	class Camera3DStore;
	class SkyboxStore;

	class Render3DStaticModelData : public Render3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hModel 
		Render3DStaticModelData(const std::string& name , ModelHandle hModel, Render3DHandle hRender3D, Render3DParameter* parameter)
			: hModel_(hModel), Render3DBaseData(name, hRender3D,parameter) { type_ = Render3D::Type::StaticModel; }

		/// @brief 初期化
		/// @param modelStore 
		/// @param device 
		void Initialize(ModelStore* modelStore, TextureStore* textureStore,LightStore* lightStore, ID3D12Device* device, Log* log);

		/// @brief 更新処理
		void Update() override;

		/// @brief リセット
		void Reset() override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam()override { return param_.get(); }

		/// @brief コマンドリストに登録
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		/// @param lightStore 
		void Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso) override;

		/// @brief コマンドリストに登録
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso) override;

		/// @brief コマンドリストに登録
		/// @param commandList 
		/// @param pso 
		void RegisterMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso) override;


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	private:

		// モデルハンドル
		ModelHandle hModel_ = 0;


	private:

		/// @brief パラメータ
		std::unique_ptr<Render3D::StaticModel::Param> param_ = nullptr;

		// テクスチャファイルパステーブル
		std::vector<std::string> textureFilePathTable_;

		
		// 座標変換リソース
		std::vector<std::unique_ptr<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>>> meshTransformationResources_;

		// マテリアルリソース
		std::vector<std::unique_ptr<ConstantBufferResource<PrimitiveModelMaterialDataForGPU>>> meshMaterialResources_;

		// シャドウマップ用座標変換リソース
		std::vector<std::unique_ptr<ConstantBufferResource<Matrix4x4>>> shadowMapTransformationResource_;

		/// @brief モーションベクター用リソース
		std::vector<std::unique_ptr<ConstantBufferResource<MotionVectorDataForGPU>>> motionVectorResources_;


	private:

		/// @brief モデルストア
		ModelStore* modelStore_ = nullptr;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;
	};
}