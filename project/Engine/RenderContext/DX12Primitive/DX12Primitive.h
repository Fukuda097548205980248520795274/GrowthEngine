#pragma once
#include <memory>
#include "PSO/PSOModel/PSOPrimitiveModel/PSOPrimitiveModel.h"
#include "Store/PrimitiveStore/PrimitiveStore.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;

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


		template <typename T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


		/// @brief 静的モデル読み込み
		/// @param model 
		/// @param modelStore 
		/// @param device 
		/// @param hModel 
		/// @param name 
		/// @param log 
		/// @return 
		PrimitiveStaticModelHandle Load(PrimitiveStaticModel* model, ModelStore* modelStore, ID3D12Device* device,
			ModelHandle hModel, const std::string& name, Log* log)
		{
			return primitiveStore_->Load(model, modelStore, device, hModel, name, log);
		}


		/// @brief 静的モデルの描画処理
		/// @param commandList 
		/// @param handle 
		void DrawStaticModel(ID3D12GraphicsCommandList* commandList, PrimitiveStaticModelHandle handle) { primitiveStore_->Register(commandList, handle, psoPrimitiveModel_.get()); }



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