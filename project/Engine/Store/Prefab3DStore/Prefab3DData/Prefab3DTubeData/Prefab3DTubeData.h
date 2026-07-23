#pragma once
#include "../Prefab3DBaseData.h"
#include "Application/PrefabInstance/PrefabInstanceTube/PrefabInstanceTube.h"
#include "DataForGPU/VertexDataForGPU/VertexDataForGPU.h"
#include "DataForGPU/PrimitiveDataForGPU/PrimitiveDataForGPU.h"
#include "Resource/RWStructuredVertexBufferResource/RWStructuredVertexBufferResource.h"
#include "Resource/ConstantBufferResource/ConstantBufferResource.h"
#include "RenderContext/ImGuiRender/ImGuiRender.h"

namespace Engine
{
	class TextureStore;
	class LightStore;
	class Camera3DStore;
	class Log;
	class SkyboxStore;
	class BaseComputePSO;

	class Prefab3DTubeData : public Prefab3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param numInstance 
		/// @param hPrefab 
		/// @param hTexture 
		/// @param parameter 
		Prefab3DTubeData(const std::string& name, uint32_t numInstance, Prefab3DHandle hPrefab3D, TextureHandle hTexture,
			BasePSOModel* pso, BaseComputePSO* csPso, Prefab3DParameter* parameter);

		/// @brief 初期化
		/// @param textureStore 
		/// @param lightStore 
		/// @param cameraStore 
		/// @param vertexResource 
		/// @param heap 
		/// @param device 
		/// @param log 
		void Initialize(TextureStore* textureStore, LightStore* lightStore, Camera3DStore* cameraStore,
			DX12Heap* heap, ID3D12Device* device,ID3D12GraphicsCommandList* commandList, Log* log);

		/// @brief 更新処理
		void Update() override;

		/// @brief リセット
		void Reset() override;


		/// @brief コマンドリストに登録する
		/// @param commandList 
		/// @param pso 
		void Register(SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList) override;

		/// @brief シャドウマップを描画する
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void DrawShadowMap(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso) override;

		/// @brief モーションベクターを描画する
		/// @param commandList 
		/// @param pso 
		void RegisterMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso) override;

		/// @brief アウトラインを描画する
		/// @param commandList 
		/// @param pso 
		void RegisterOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso) override;


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
		std::unique_ptr<Prefab3D::Tube::Base::Param> param_ = nullptr;


		// テクスチャハンドル
		TextureHandle hTexture_ = 0;

		// テクスチャファイルパス
		std::string textureFilePath_{};

		/// @brief CSTubePSO
		BaseComputePSO* csTubePSO_ = nullptr;



	private:

		// 最大分割数
		static constexpr uint32_t kMaxSlices = 32;

		uint32_t perSlices_ = 0;

		/// @brief 頂点リソース
		std::unique_ptr<RWStructuredVertexBufferResource<VertexDataForGPU>> vertexResource_;

		/// @brief インデックスリソース
		std::unique_ptr<RWStructuredVertexBufferResource<uint32_t>> indexResource_;


	private:

		/// @brief プリミティブリソース
		std::unique_ptr<StructuredBufferResource<Prefab::TubeDataForGPU>> primitiveResource_;

		/// @brief 分割リソース
		std::unique_ptr<ConstantBufferResource<PrimitiveDataForGPU::TubeDivisionDataForGPU>> divisionResource_;

		/// @brief ビュー変換用リソース
		std::unique_ptr<ConstantBufferResource<Matrix4x4>> viewResource_;

		/// @brief シャドウマップ座標変換用リソース
		std::unique_ptr<StructuredBufferResource<Matrix4x4>> shadowMapTransformationResource_;

		/// @brief モーションベクターリソース
		std::unique_ptr<StructuredBufferResource<MotionVectorDataForGPU>> motionVectorResource_;

		/// @brief アウトラインリソース
		std::unique_ptr<StructuredBufferResource<PrefabOutlineDataForGPU>> outlineResource_;


	private:

		/// @brief インスタンスのドローコール
		void DrawCallInstance(const Engine::Prefab3D::Tube::Instance::Param* param);

		/// @brief インスタンステーブル
		std::list<std::unique_ptr<PrefabInstanceTube>> instanceTable_;


	private:

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;

		/// @brief カメラストア
		Camera3DStore* cameraStore_ = nullptr;
	};
}