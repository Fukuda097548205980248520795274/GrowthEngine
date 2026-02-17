#pragma once
#include <memory>
#include "PSO/PSOModel/PSOPrefabSprite/PSOPrefabSprite.h"
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
		/// @param shaderCompiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* shaderCompiler, Log* log);

		/// @brief 更新処理
		void Update();

		/// @brief スプライトを読み込む
		/// @param name 
		/// @param hTexture 
		/// @param numInstance 
		/// @param textureStore 
		/// @param heap 
		/// @param device 
		/// @param log 
		PrefabSpriteHandle LoadSprite(const std::string& name, TextureHandle hTexture,uint32_t numInstance, 
			TextureStore* textureStore,DX12Heap* heap, ID3D12Device* device, Log* log)
		{
			return prefabSpriteStore_->Load(name, hTexture, numInstance, textureStore, heap, device, log);
		}

		/// @brief リセット
		void Reset();


		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:

		// スプライト用プレハブ頂点シェーダ
		ComPtr<IDxcBlob> spritePrefabVS_ = nullptr;

		// スプライト用プレハブピクセルシェーダ
		ComPtr<IDxcBlob> spritePrefabPS_ = nullptr;


	private:

		/// @brief スプライト用プレハブPSO
		std::unique_ptr<PSOPrefabSprite> psoPrefabSprite_ = nullptr;



	private:

		/// @brief スプライト用プレハブストア
		std::unique_ptr<PrefabSpriteStore> prefabSpriteStore_ = nullptr;
	};
}