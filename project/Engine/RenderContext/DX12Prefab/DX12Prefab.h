#pragma once
#include <memory>
#include "PSO/PSOModel/PSOPrefabSprite/PSOPrefabSprite.h"
#include "PSO/PSOModel/PSOPrefabPrimitive/PSOPrefabPrimitive.h"

#include "Store/Prefab3DStore/Prefab3DStore.h"
#include "Store/Prefab2DStore/Prefab2DStore.h"

#include "Application/PrefabBase/PrefabBaseCube/PrefabBaseCube.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;
	class BasePSOShadowMap;
	class ModelStore;
	class TextureStore;
	class SkyboxStore;

	class DX12Prefab
	{
	public:

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
		void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, ShaderCompiler* compiler, DX12Heap* heap,
			ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore,
			LightStore* lightStore, Camera3DStore* cameraStore, Log* log);

		/// @brief 更新処理
		void Update();

		/// @brief リセット
		void Reset();


		/// @brief プリミティブを読み込む
		/// @param name 
		/// @param type 
		/// @param numInstance 
		/// @param hModel 
		/// @param hAnimation 
		/// @param hSkeleton 
		/// @param device 
		/// @param commandList 
		/// @param log 
		/// @return 
		Prefab3DHandle LoadPrimitive(const std::string& name, Prefab3D::Type type, uint32_t numInstance,
			TextureHandle hTexture, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton,
			ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log)
		{
			return prefab3DStore_->Load(device, commandList, hTexture, hModel, hAnimation, hSkeleton, name, numInstance, type, log);
		}

		/// @brief スプライトを読み込む
		/// @param name 
		/// @param hTexture 
		/// @param numInstance 
		/// @param textureStore 
		/// @param heap 
		/// @param device 
		/// @param log 
		Prefab2DHandle LoadSprite(const std::string& name, TextureHandle hTexture,uint32_t numInstance, 
			TextureStore* textureStore,Camera2DStore* cameraStore, DX12Heap* heap, ID3D12Device* device, Log* log)
		{
			return prefab2DStore_->Load(name, hTexture, numInstance, textureStore,cameraStore, heap, device, log);
		}


		/// @brief シャドウマップの描画処理
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);


		/// @brief プレハブ描画処理
		/// @param skyboxStore 
		/// @param commandList 
		void DrawPrefab(SkyboxStore* skyboxStore, ID3D12GraphicsCommandList* commandList);


		/// @brief プリミティブのパラメータを取得する
		/// @tparam T 
		/// @param hPrefabPrimitive 
		/// @return 
		template<typename T>
		T* GetPrefab3DParam(Prefab3DHandle hPrefab3D) { return prefab3DStore_->GetParam<T>(hPrefab3D); }

		/// @brief プリミティブのパラメータを取得する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* GetPrefab3DParam(const std::string& name) { return prefab3DStore_->GetParam<T>(name); }

		/// @brief スプライトのパラメータを取得する
		/// @return 
		Prefab2D::Sprite::Base::Param* GetPrefab2DParam(Prefab2DHandle hPrefab2D) { return prefab2DStore_->GetParam(hPrefab2D); }

		/// @brief スプライトのパラメータを取得する
		/// @param name 
		/// @return 
		Prefab2D::Sprite::Base::Param* GetPrefab2DParam(const std::string& name) { return prefab2DStore_->GetParam(name); }


		/// @brief プリミティブ用インスタンスを作成する
		/// @tparam T 
		/// @param hPrefabPrimitive 
		/// @return 
		template<typename T>
		T* CreatePrefab3DInstance(Prefab3DHandle hPrefabPrimitive) { return prefab3DStore_->CreateInstance<T>(hPrefabPrimitive); }

		/// @brief プリミティブ用インスタンスを作成する
		/// @tparam T 
		/// @param name 
		/// @return 
		template<typename T>
		T* CreatePrefab3DInstance(const std::string& name) { return prefab3DStore_->CreateInstance<T>(name); }


		/// @brief スプライト用インスタンスを作成する
		/// @param hPrefabSprite 
		/// @return 
		PrefabInstanceSprite* CreatePrefab2DInstance(Prefab2DHandle hPrefabSprite) { return prefab2DStore_->CreateInstance(hPrefabSprite); }

		/// @brief スプライト用インスタンスを作成する
		/// @param name 
		/// @return 
		PrefabInstanceSprite* CreatePrefab2DInstance(const std::string& name) { return prefab2DStore_->CreateInstance(name); }


		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance();

		/// @brief プレハブリセット
		void PrefabReset();

		/// @brief デバッグ用パラメータ
		void DebugParameter();

		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;



	public:

#ifdef _DEVELOPMENT

		/// @brief 立方体を描画する
		/// @param position 
		/// @param rotate 
		/// @param scale 
		/// @param color 
		void DrawDebugCube(const Vector3& position, const Vector3& rotate, const Vector3& scale, const Vector4& color);

#endif

	private:

		// プリミティブ用プレハブ頂点シェーダ
		ComPtr<IDxcBlob> primitivePrefabVS_ = nullptr;

		// プリミティブ用プレハブピクセルシェーダ
		ComPtr<IDxcBlob> primitivePrefabPS_ = nullptr;


		// スプライト用プレハブ頂点シェーダ
		ComPtr<IDxcBlob> spritePrefabVS_ = nullptr;

		// スプライト用プレハブピクセルシェーダ
		ComPtr<IDxcBlob> spritePrefabPS_ = nullptr;


	private:

		/// @brief プリミティブ用プレハブPSO
		std::unique_ptr<PSOPrefabPrimitive> psoPrefabPrimitive_ = nullptr;

		/// @brief スプライト用プレハブPSO
		std::unique_ptr<PSOPrefabSprite> psoPrefabSprite_ = nullptr;



	private:

		/// @brief プレハブ3Dストア
		std::unique_ptr<Prefab3DStore> prefab3DStore_ = nullptr;

		/// @brief プレハブ2Dストア
		std::unique_ptr<Prefab2DStore> prefab2DStore_ = nullptr;


#ifdef _DEVELOPMENT

	private:

		/// @brief 立方体
		std::unique_ptr<PrefabBaseCube> cube_ = nullptr;

		/// @brief 立方体インスタンスリスト
		std::vector<PrefabInstanceCube*> cubeInstances_;

		/// @brief 立方体描画数
		int32_t cubeNumDraw_ = 0;

#endif
	};
}