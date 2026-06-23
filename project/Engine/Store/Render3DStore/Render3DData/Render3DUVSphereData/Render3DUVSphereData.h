#pragma once
#include "../Render3DBaseData.h"
#include "DataForGPU/VertexDataForGPU/VertexDataForGPU.h"
#include "Resource/VertexBufferResource/VertexBufferResource.h"
#include "Resource/IndexBufferResource/IndexBufferResource.h"

namespace Engine
{
	class TextureStore;
	class LightStore;
	class SkyboxStore;
	class DX12Heap;
	class Log;

	class Render3DUVSphereData : public Render3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param hTexture 
		Render3DUVSphereData(const std::string& name, TextureHandle hTexture, Render3DHandle hRender3D, Render3DParameter* parameter)
			: hTexture_(hTexture), Render3DBaseData(name, hRender3D, parameter) {
			type_ = Render3D::Type::UVSphere;
		}

		/// @brief 初期化
		/// @param modelStore 
		/// @param device 
		void Initialize(TextureStore* textureStore, LightStore* lightStore, ID3D12Device* device, Log* log);

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

		// テクスチャハンドル
		TextureHandle hTexture_ = 0;


	private:

		/// @brief パラメータ
		std::unique_ptr<Render3D::UVSphere::Param> param_ = nullptr;

		// テクスチャファイルパス
		std::string textureFilePath_;


	private:

		/// @brief 頂点計算
		void VertexCalculation();

		// 頂点リソース
		std::unique_ptr<VertexBufferResource<VertexDataForGPU>> vertexResource_ = nullptr;

		// インデックスリソース
		std::unique_ptr<IndexBufferResource> indexResource_ = nullptr;

		// 分割リソース
		std::unique_ptr<ConstantBufferResource<PrimitiveDataForGPU::UVSphereDivisionDataForGPU>> divisionResource_ = nullptr;

		// スライスの最大値
		static constexpr int32_t kMaxSlices = 32;

		// リングの最大値
		static constexpr int32_t kMaxRings = 16;

		int32_t preSlices_ = 0;
		int32_t preRings_ = 0;

	private:

		// 座標変換リソース
		std::unique_ptr<ConstantBufferResource<PrimitiveModelTransformationDataForGPU>> transformationResources_;

		// マテリアルリソース
		std::unique_ptr<ConstantBufferResource<PrimitiveModelMaterialDataForGPU>> materialResources_;

		// シャドウマップ用座標変換リソース
		std::unique_ptr<ConstantBufferResource<Matrix4x4>> shadowMapTransformationResource_;

		/// @brief モーションベクトルリソース
		std::unique_ptr<ConstantBufferResource<MotionVectorDataForGPU>> motionVectorResource_;


	private:

		// CSUV球PSO
		BaseComputePSO* psoUVSphere_ = nullptr;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;
	};
}