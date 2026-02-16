#pragma once
#include <memory>
#include "PSO/PSOModel/PSOPrimitiveModel/PSOPrimitiveModel.h"
#include "Store/PrimitiveStore/PrimitiveStore.h"
#include "Data/PrimitiveData/PrimitiveData.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;
	class BasePSOShadowMap;
	class ModelStore;
	class TextureStore;

	class DX12Primitive
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
		PrimitiveHandle Load(ModelStore* modelStore, TextureStore* textureStore, AnimationStore* animationStore,SkeletonStore* skeletonStore, 
			ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
			ModelHandle hModel, AnimationHandle hAnimation,SkeletonHandle hSkeleton, DX12Heap* heap, const std::string& name, Primitive::Type type, Log* log)
		{
			return primitiveStore_->Load(modelStore, textureStore, animationStore, skeletonStore, device,commandList, hModel, hAnimation, hSkeleton, heap, name, type, log);
		}


		/// @brief モデルの描画処理
		/// @param commandList 
		/// @param handle 
		void DrawModel(const Matrix4x4& viewProjection, ID3D12GraphicsCommandList* commandList, PrimitiveHandle handle, LightStore* lightStore) 
		{
			primitiveStore_->Register(viewProjection, commandList, handle, psoPrimitiveModel_.get() , lightStore); 
		}


		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;

	private:

		// モデル用プリミティブ頂点シェーダ
		ComPtr<IDxcBlob> primitiveModelVS_ = nullptr;

		// モデル用プリミティブピクセルシェーダ
		ComPtr<IDxcBlob> primitiveModelPS_ = nullptr;


	private:

		// モデル用プリミティブPSO
		std::unique_ptr<PSOPrimitiveModel> psoPrimitiveModel_ = nullptr;


	private:

		/// @brief プリミティブストア
		std::unique_ptr<PrimitiveStore> primitiveStore_ = nullptr;
	};
}