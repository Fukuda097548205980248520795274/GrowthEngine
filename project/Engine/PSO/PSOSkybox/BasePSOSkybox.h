#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <dxcapi.h>
#include <cstdint>

namespace Engine
{
	class ShaderCompiler;
	class Log;

	class BasePSOSkybox
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		virtual void Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log) = 0;

		/// @brief コマンドリストに登録する
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList);


		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


	protected:

		// 頂点シェーダのバイナリデータ
		ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;

		// ピクセルシェーダのバイナリデータ
		ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

		// シグネチャのバイナリデータ
		ComPtr<ID3DBlob> signatureBlob_ = nullptr;

		// エラーのバイナリデータ
		ComPtr<ID3DBlob> errorBlob_ = nullptr;

		// ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;

		// PSO
		ComPtr<ID3D12PipelineState> graphicsPipelineState_ = nullptr;
	};
}