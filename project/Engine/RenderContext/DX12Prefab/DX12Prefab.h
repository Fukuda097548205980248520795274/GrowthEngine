#pragma once
#include <memory>
#include "PSO/PSOModel/PSOPrefabSprite/PSOPrefabSprite.h"
#include "PSO/PSOModel/PSOPrefabPrimitive/PSOPrefabPrimitive.h"

#include "Store/PrefabPrimitiveStore/PrefabPrimitiveStore.h"
#include "Store/PrefabSpriteStore/PrefabSpriteStore.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;
	class BasePSOShadowMap;
	class ModelStore;
	class TextureStore;

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
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, DX12Heap* heap,
			ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore, SkeletonStore* skeletonStore,
			LightStore* lightStore, Camera3DStore* cameraStore, Log* log);

		/// @brief 更新処理
		void Update();


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
		PrefabPrimitiveHandle LoadPrimitive(const std::string& name, Prefab::Type type, uint32_t numInstance, ModelHandle hModel, AnimationHandle hAnimation, SkeletonHandle hSkeleton,
			ID3D12Device* device, ID3D12GraphicsCommandList* commandList, Log* log)
		{
			return prefabPrimitiveStore_->Load(device, commandList, hModel, hAnimation, hSkeleton, name, numInstance, type, log);
		}

		/// @brief スプライトを読み込む
		/// @param name 
		/// @param hTexture 
		/// @param numInstance 
		/// @param textureStore 
		/// @param heap 
		/// @param device 
		/// @param log 
		PrefabSpriteHandle LoadSprite(const std::string& name, TextureHandle hTexture,uint32_t numInstance, 
			TextureStore* textureStore,Camera2DStore* cameraStore, DX12Heap* heap, ID3D12Device* device, Log* log)
		{
			return prefabSpriteStore_->Load(name, hTexture, numInstance, textureStore,cameraStore, heap, device, log);
		}


		/// @brief シャドウマップの描画処理
		/// @param viewProjection 
		/// @param commandList 
		/// @param pso 
		void ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);


		/// @brief プリミティブ用プレハブを描画する
		/// @param hPrefabPrimitive 
		/// @param commandList 
		void DrawPrefabPrimitive(PrefabPrimitiveHandle hPrefabPrimitive, ID3D12GraphicsCommandList* commandList) { prefabPrimitiveStore_->Register(hPrefabPrimitive, commandList, psoPrefabPrimitive_.get()); }

		/// @brief スプライト用プレハブでの描画処理
		/// @param hPrefabSprite 
		/// @param commandList 
		void DrawPrefabSprite(PrefabSpriteHandle hPrefabSprite, ID3D12GraphicsCommandList* commandList) { prefabSpriteStore_->Register(hPrefabSprite, commandList, psoPrefabSprite_.get()); }


		/// @brief スプライトのパラメータを取得する
		/// @return 
		Prefab::Sprite::Base::Param* GetSpriteParam(PrefabSpriteHandle hPrefabSprite) { return prefabSpriteStore_->GetParam(hPrefabSprite); }


		/// @brief プリミティブ用インスタンスを作成する
		/// @tparam T 
		/// @param hPrefabPrimitive 
		/// @return 
		template<typename T>
		T* CreatePrimitiveInstance(PrefabPrimitiveHandle hPrefabPrimitive) { return prefabPrimitiveStore_->CreateInstance<T>(hPrefabPrimitive); }

		/// @brief スプライト用インスタンスを作成する
		/// @param hPrefabSprite 
		/// @return 
		PrefabInstanceSprite* CreateSpriteInstance(PrefabSpriteHandle hPrefabSprite) { return prefabSpriteStore_->CreateInstance(hPrefabSprite); }


		/// @brief 全てのインスタンスを削除する
		void DestroyAllInstance();

		/// @brief リセット
		void Reset();


		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

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

		/// @brief プリミティブ用プレハブストア
		std::unique_ptr<PrefabPrimitiveStore> prefabPrimitiveStore_ = nullptr;

		/// @brief スプライト用プレハブストア
		std::unique_ptr<PrefabSpriteStore> prefabSpriteStore_ = nullptr;
	};
}