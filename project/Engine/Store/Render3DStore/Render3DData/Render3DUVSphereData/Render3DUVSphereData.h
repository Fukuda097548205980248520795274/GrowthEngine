#pragma once
#include "../Render3DBaseData.h"
#include "DataForGPU/VertexDataForGPU/VertexDataForGPU.h"

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
		void Initialize(TextureStore* textureStore, LightStore* lightStore, DX12Heap* heap,
			ID3D12Device* device,ID3D12GraphicsCommandList* commandList, BaseComputePSO* psoUVSphere, Log* log);

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
		void Register(Camera3DStore* cameraStore, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList, BasePSOModel* pso);

		/// @brief コマンドリストに登録
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void Register(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);


	public:

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;

		/// @brief デバッグ用レイピッキング
		/// @param ray 
		/// @param pickList 
		void DebugRayPicker(const Collision3D::Ray& ray, std::vector<std::pair<float, DebugData::DebugGuizmoData*>>& pickList) override;

		/// @brief Guizmo操作
		/// @param cameraStore 
		void DebugGuizmo(Camera3DStore* cameraStore) override;


	private:

		// テクスチャハンドル
		TextureHandle hTexture_ = 0;


	private:

		/// @brief パラメータ
		std::unique_ptr<Render3D::UVSphere::Param> param_ = nullptr;

		// テクスチャファイルパス
		std::string textureFilePath_;


	private:

		// 頂点リソース
		std::unique_ptr<RWStructuredVertexBufferResource<VertexDataForGPU>> vertexResource_ = nullptr;

		// インデックスリソース
		std::unique_ptr<RWStructuredVertexBufferResource<uint32_t>> indexResource_ = nullptr;

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


	private:

		// シャドウマップ用座標変換リソース
		std::unique_ptr<ConstantBufferResource<Matrix4x4>> shadowMapTransformationResource_;


	private:

		// CSUV球PSO
		BaseComputePSO* psoUVSphere_ = nullptr;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;
	};
}