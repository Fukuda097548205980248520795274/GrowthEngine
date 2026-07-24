#pragma once
#include "Prefab3DData/Prefab3DBaseData.h"
#include "Math/Matrix/Matrix4x4/Matrix4x4.h"
#include <memory>
#include <vector>

#include "Parameter/Prefab3DParameter/Prefab3DParameter.h"

#include "PSO/PSOModel/PSOPrefab3D/PSOPrefab3D.h"
#include "PSO/PSOModel/PSOPrefab3DTube/PSOPrefab3DTube.h"
#include "PSO/ComputePSO/ComputePSOTube/ComputePSOTube.h"

#include "Resource/VertexBufferResource/CubeVertexResource/CubeVertexResource.h"

namespace Engine
{
	class ModelStore;
	class TextureStore;
	class AnimationStore;
	class SkeletonStore;
	class Camera3DStore;
	class LightStore;
	class ShaderCompiler;
	class DX12Heap;
	class Log;
	class BasePSOShadowMap;
	class BasePSOModel;
	class SkyboxStore;

	class Prefab3DStore
	{
	public:

		/// @brief コンストラクタ
		Prefab3DStore();

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param heap 
		/// @param modelStore 
		/// @param textureStore 
		/// @param animationStore 
		/// @param skeletonStore 
		/// @param lightStore 
		/// @param cameraStore 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap,
			ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore,
			LightStore* lightStore, Camera3DStore* cameraStore, Log* log);

		/// @brief プレハブの読み込み
		/// @param device 
		/// @param commandList 
		/// @param hModel 
		/// @param hAnimation 
		/// @param hSkeleton 
		/// @param name 
		/// @param numInstance 
		/// @param type 
		/// @param log 
		Prefab3DHandle Load(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
			TextureHandle hTexture, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton,
			const std::string& name, uint32_t numInstance, Prefab3D::Type type, Log* log);

		/// @brief 更新処理
		void Update();

		/// @brief すべての描画処理
		/// @param skyboxStore 
		/// @param commandList 
		/// @param pso 
		void AllDrawPrefab(SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList);

		/// @brief プレハブの描画処理
		/// @param hPrefab3D 
		/// @param skyboxStore 
		/// @param commandList 
		/// @param pso 
		void DrawPrefab(Prefab3DHandle hPrefab3D, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList);

		/// @brief プレハブの描画処理
		/// @param name 
		/// @param skyboxStore 
		/// @param commandList 
		/// @param pso 
		void DrawPrefab(const std::string& name, SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList);

		/// @brief シャドウマップの描画処理
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);

		/// @brief モーションベクターの描画処理
		/// @param commandList 
		/// @param pso 
		void DrawMotionVector(ID3D12GraphicsCommandList* commandList, BasePSOMotionVector* pso);

		/// @brief アウトラインの描画処理
		/// @param commandList 
		/// @param pso 
		void DrawOutline(ID3D12GraphicsCommandList* commandList, BasePSOOutline* pso);

		/// @brief リセット
		void Reset();

		/// @brief シーン前のリセット
		void PerSceneReset();

		/// @brief インスタンスを生成する
		/// @tparam T 
		/// @param hPrefabPrimitive 
		/// @return 
		template<typename T>
		T* CreateInstance(Prefab3DHandle hPrefab3D)
		{
			Prefab3DBaseData* data = dataTable_[hPrefab3D].get();
			return static_cast<T*>(data->CreateInstance());
		}

		/// @brief インスタンスを作成する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* CreateInstance(const std::string& name)
		{
			Prefab3DBaseData* data = dataTable_[nameTable_[name]].get();
			return static_cast<T*>(data->CreateInstance());
		}

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param hPrefabPrimitive 
		/// @return 
		template<typename T>
		T* GetParam(Prefab3DHandle hPrefab3D)
		{
			Prefab3DBaseData* data = dataTable_[hPrefab3D].get();
			return static_cast<T*>(data->GetParam());
		}

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetParam(const std::string& name)
		{
			Prefab3DBaseData* data = dataTable_[nameTable_[name]].get();
			return static_cast<T*>(data->GetParam());
		}

		/// @brief デバッグ用パラメータ
		void DebugParameter();


		// Microsoft::WRL 省略
		template<typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


	private:

		/// @brief データテーブル
		std::vector<std::unique_ptr<Prefab3DBaseData>> dataTable_;

		/// @brief 名前テーブル
		std::map<std::string, Prefab3DHandle> nameTable_;

		// パラメータ
		std::unique_ptr<Prefab3DParameter> parameter_ = nullptr;


	private:

		// 3Dプレハブ頂点シェーダ
		ComPtr<IDxcBlob> prefab3DVS_ = nullptr;

		/// @brief 3Dプレハブチューブ頂点シェーダ
		ComPtr<IDxcBlob> prefab3DTubeVS_ = nullptr;

		// 3Dプレハブピクセルシェーダ
		ComPtr<IDxcBlob> prefab3DPS_ = nullptr;


	private:

		/// @brief 3Dプレハブ用PSO
		std::unique_ptr<PSOPrefab3D> psoPrefab3D_ = nullptr;

		/// @brief 3Dプレハブチューブ用PSO
		std::unique_ptr<PSOPrefab3DTube> psoPrefab3DTube_ = nullptr;

		/// @brief CSチューブ用PSO
		std::unique_ptr<ComputePSOTube> computePSOTube_ = nullptr;


	private:

		/// @brief 立方体頂点リソース
		std::unique_ptr<CubeVertexResource> cubeVertexResource_ = nullptr;



	private:

		/// @brief ヒープ
		DX12Heap* heap_ = nullptr;

		/// @brief モデルストア
		ModelStore* modelStore_ = nullptr;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;

		/// @brief アニメーションストア
		AnimationStore* animationStore_ = nullptr;

		/// @brief スケルトンストア
		SkeletonStore* skeletonStore_ = nullptr;

		/// @brief カメラストア
		Camera3DStore* cameraStore_ = nullptr;

		/// @brief ライトストア
		LightStore* lightStore_ = nullptr;
	};
}