#pragma once
#include <memory>
#include "PSO/PSOModel/PSOPrimitiveModel/PSOPrimitiveModel.h"

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
	};
}