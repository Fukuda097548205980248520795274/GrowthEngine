#pragma once
#include "../Render3DBaseData.h"
#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"

namespace Engine
{
	class TextureStore;
	class LightStore;
	class SkyboxStore;
	class Log;

	class Render3DCylinderData : public Render3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hTexture 
		Render3DCylinderData(const std::string& name, TextureHandle hTexture, Render3DHandle hRender3D)
			: hTexture_(hTexture), Render3DBaseData(name, hRender3D) {
			type_ = Render3D::Type::Cylinder;
		}

		/// @brief 初期化
		/// @param modelStore 
		/// @param device 
		void Initialize(TextureStore* textureStore, LightStore* lightStore, 
			ID3D12Device* device, Log* log);

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

		/// @brief アウトライン用のコマンドリストに登録
		/// @param commandList 
		/// @param cameraStore 
		/// @param pso 
		void RegisterOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso) override;


	private:

		// テクスチャハンドル
		TextureHandle hTexture_ = 0;


	private:

		/// @brief パラメータ
		std::unique_ptr<Render3D::Cylinder::Param> param_ = nullptr;

		// テクスチャファイルパス
		std::string textureFilePath_;


	private:

		/// @brief 頂点計算
		void VertexCalculate();

		// 頂点リソース
		std::unique_ptr<VertexBufferResource<VertexDataForGPU>> vertexResource_ = nullptr;

		// インデックスリソース
		std::unique_ptr<IndexBufferResource> indexResource_ = nullptr;

		// スライスの最大値
		static constexpr int32_t kMaxSlices = 32;

		// 円柱の前回の値
		int32_t preSlices_ = 0;
		float preTopRadius_ = 0.0f;
		float preBottomRadius_ = 0.0f;
		float preHeight_ = 0.0f;

	private:

		// 座標変換リソース
		std::unique_ptr<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>> transformationResources_;

		// マテリアルリソース
		std::unique_ptr<ConstantBufferResource<PrimitiveModelMaterialDataForGPU>> materialResources_;

		// シャドウマップ用座標変換リソース
		std::unique_ptr<ConstantBufferResource<Matrix4x4>> shadowMapTransformationResource_;

		/// @brief モーションベクトルリソース
		std::unique_ptr<ConstantBufferResource<MotionVectorDataForGPU>> motionVectorResource_;

		// アウトライン用座標変換リソース
		std::unique_ptr<ConstantBufferResource<Matrix4x4>> outlineTransformationResource_;

		// アウトライン用色リソース
		std::unique_ptr<ConstantBufferResource<Vector4>> outlineColorResource_;


	private:

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;
	};
}