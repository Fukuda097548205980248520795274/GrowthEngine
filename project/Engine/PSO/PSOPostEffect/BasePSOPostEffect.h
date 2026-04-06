#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <dxcapi.h>

namespace Engine
{
	class Log;
	class ShaderCompiler;

	class BasePSOPostEffect
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BasePSOPostEffect() = default;

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param vertexShaderBlob 
		/// @param log 
		virtual void Initialize(ID3D12Device* device, ShaderCompiler* compiler, IDxcBlob* vertexShaderBlob, Log* log) = 0;

		/// @brief コマンドリストに登録する
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList);


		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


	protected:


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
