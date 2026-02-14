#pragma once
#include <memory>
#include "PSO/PSOModel/PSOPrimitiveModel/PSOPrimitiveModel.h"
#include "Store/PrimitiveStore/PrimitiveStore.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;
	class BasePSOShadowMap;

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
		void Update(const Matrix4x4& viewProjection);

		/// @brief シャドウマップ用更新処理
		/// @param viewProjection 
		void ShadowMapUpdate(const Matrix4x4& viewProjection);

		/// @brief シャドウアップ用描画処理
		/// @param commandList 
		void ShadowMapDraw(ID3D12GraphicsCommandList* commandList, BasePSOShadowMap* pso);


		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


		/// @brief プリミティブ読み込み
		/// @param modelStore 
		/// @param textureStore 
		/// @param device 
		/// @param hModel 
		/// @param name 
		/// @param type 
		/// @param log 
		/// @return 
		PrimitiveHandle Load(ModelStore* modelStore, TextureStore* textureStore, ID3D12Device* device,
			ModelHandle hModel, const std::string& name, const std::string& type, Log* log)
		{
			return primitiveStore_->Load(modelStore, textureStore, device, hModel, name, type, log);
		}


		/// @brief モデルの描画処理
		/// @param commandList 
		/// @param handle 
		void DrawModel(ID3D12GraphicsCommandList* commandList, PrimitiveHandle handle, LightStore* lightStore) { primitiveStore_->Register(commandList, handle, psoPrimitiveModel_.get() , lightStore); }



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