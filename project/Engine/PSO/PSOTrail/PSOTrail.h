#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <dxcapi.h>
#include <cstdint>
#include "../EnumBlendMode/EnumBlendMode.h"

namespace Engine
{
	class ShaderCompiler;
	class Log;

	class PSOTrail
	{
	public:

		/// @brief 初期化
		/// @param device 
		/// @param compiler 
		/// @param log 
		void Initialize(ID3D12Device* device, ShaderCompiler* compiler, Log* log);

		/// @brief ブレンドモードの設定
		/// @param blendMode 
		void SetBlendMode(BlendMode blendMode) { blendMode_ = static_cast<int>(blendMode); }

		/// @brief コマンドリストに登録する
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList) const;

		/// @brief ブレンドモードを初期化する
		void ResetBlendMode() { blendMode_ = static_cast<int>(BlendMode::kNormal); }


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
		ComPtr<ID3D12PipelineState> graphicsPipelineState_[static_cast<int>(BlendMode::kNumBlendMode)];



		// 現在のブレンドモード
		int32_t blendMode_ = static_cast<int>(BlendMode::kNormal);
	};
}