#pragma once
#include <memory>
#include "PSO/PSOModel/PSOPrimitive/PSOPrimitive.h"
#include "PSO/PSOModel/PSOSprite/PSOSprite.h"
#include "Store/PrimitiveStore/PrimitiveStore.h"
#include "Store/SpriteStore/SpriteStore.h"
#include "Data/PrimitiveData/PrimitiveData.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;
	class BasePSOShadowMap;
	class ModelStore;
	class TextureStore;

	class DX12Model
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param shaderCompiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* shaderCompiler, Log* log);

		/// @brief 更新処理
		/// @param viewProjection 
		void Update(ID3D12GraphicsCommandList* commandList);

		/// @brief シャドウアップ用描画処理
		/// @param commandList 
		void ShadowMapDraw(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);

		/// @brief パラメータを取得する
		/// @tparam T 
		/// @param handle 
		/// @return 
		template<typename T>
		T* GetParam(PrimitiveHandle handle) { return primitiveStore_->GetParam<T>(handle); }

		/// @brief プリミティブ読み込み
		/// @param modelStore 
		/// @param textureStore 
		/// @param device 
		/// @param hModel 
		/// @param name 
		/// @param type 
		/// @param log 
		/// @return 
		PrimitiveHandle LoadPrimitive(ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore,SkeletonStore* skeletonStore, 
			ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
			ModelHandle hModel, AnimationHandle hAnimation,SkeletonHandle hSkeleton, DX12Heap* heap, const std::string& name, Primitive::Type type, Log* log)
		{
			return primitiveStore_->Load(modelStore, textureStore, animationStore, skeletonStore, device,commandList, hModel, hAnimation, hSkeleton, heap, name, type, log);
		}

		/// @brief スプライト読み込み
		/// @param textureStore 
		/// @param device 
		/// @param hTexture 
		/// @param name 
		/// @param log 
		/// @return 
		SpriteHandle LoadSprite(TextureStore* textureStore, ID3D12Device* device, TextureHandle hTexture, const std::string& name, Log* log)
		{
			return spriteStore_->Load(name, hTexture, textureStore, device, log);
		}


		/// @brief プリミティブの描画処理
		/// @param commandList 
		/// @param handle 
		void DrawPrimitive(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, PrimitiveHandle handle, LightStore* lightStore) 
		{
			primitiveStore_->Register(viewProjection, commandList, handle, psoPrimitive_.get() , lightStore); 
		}

		/// @brief スプライトの描画処理
		/// @param hSprite 
		/// @param viewProjection 
		/// @param commandList 
		void DrawSprite(SpriteHandle hSprite, const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList)
		{
			spriteStore_->Register(hSprite, viewProjection, commandList, psoSprite_.get());
		}


		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:

		// プリミティブ頂点シェーダ
		ComPtr<IDxcBlob> primitiveVS_ = nullptr;

		// プリミティブピクセルシェーダ
		ComPtr<IDxcBlob> primitivePS_ = nullptr;


		// スプライト頂点シェーダ
		ComPtr<IDxcBlob> spriteVS_ = nullptr;

		// スプライトピクセルシェーダ
		ComPtr<IDxcBlob> spritePS_ = nullptr;


	private:

		// プリミティブPSO
		std::unique_ptr<PSOPrimitive> psoPrimitive_ = nullptr;

		// スプライトPSO
		std::unique_ptr<PSOSprite> psoSprite_ = nullptr;


	private:

		/// @brief プリミティブストア
		std::unique_ptr<PrimitiveStore> primitiveStore_ = nullptr;

		/// @brief スプライトストア
		std::unique_ptr<SpriteStore> spriteStore_ = nullptr;
	};
}