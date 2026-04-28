#pragma once
#include "../Prefab3DBaseData.h"
#include "Application/PrefabInstance/PrefabInstanceCube/PrefabInstanceCube.h"
#include "DataForGPU/VertexDataForGPU/VertexDataForGPU.h"

#include "RenderContext/ImGuiRender/ImGuiRender.h"

namespace Engine
{
	class TextureStore;
	class LightStore;
	class Camera3DStore;
	class Log;
	class SkyboxStore;
	class CubeVertexResource;

	class Prefab3DCubeData : public Prefab3DBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param numInstance 
		/// @param hPrefab 
		/// @param hTexture 
		/// @param parameter 
		Prefab3DCubeData(const std::string& name, uint32_t numInstance, Prefab3DHandle hPrefab3D, TextureHandle hTexture, Prefab3DParameter* parameter);

		/// @brief 初期化
		/// @param textureStore 
		/// @param lightStore 
		/// @param cameraStore 
		/// @param vertexResource 
		/// @param heap 
		/// @param device 
		/// @param log 
		void Initialize(TextureStore* textureStore, LightStore* lightStore, Camera3DStore* cameraStore,
			CubeVertexResource* vertexResource, DX12Heap* heap, ID3D12Device* device, Log* log);

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

		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance() override;

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	private:

		/// @brief パラメータ
		std::unique_ptr<Prefab3D::Cube::Base::Param> param_ = nullptr;


		// テクスチャハンドル
		TextureHandle hTexture_ = 0;

		// テクスチャファイルパス
		std::string textureFilePath_{};





	private:

		/// @brief プリミティブリソース
		std::unique_ptr<StructuredBufferResource<Prefab::PrimitiveDataForGPU>> primitiveResource_;

		/// @brief シャドウマップ座標変換用リソース
		std::unique_ptr<StructuredBufferResource<Matrix4x4>> shadowMapTransformationResource_;

		/// @brief モーションベクターリソース
		std::unique_ptr<StructuredBufferResource<MotionVectorDataForGPU>> motionVectorResource_;


		/// @brief 立方体頂点リソース
		CubeVertexResource* vertexResource_ = nullptr;


	private:

		/// @brief インスタンスのドローコール
		void DrawCallInstance(const Engine::Prefab3D::Cube::Instance::Param* param);

		/// @brief インスタンステーブル
		std::list<std::unique_ptr<PrefabInstanceCube>> instanceTable_;


	private:

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;

		/// @brief カメラストア
		Camera3DStore* cameraStore_ = nullptr;
	};
}