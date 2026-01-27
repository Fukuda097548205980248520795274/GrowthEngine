#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <dxcapi.h>
#include <cstdint>
#include "../EnumBlendMode/EnumBlendMode.h"

namespace Engine
{
	class Log;
	class ShaderCompiler;

	class BasePSOModel
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BasePSOModel() = default;

		/// @brief 初期化
		/// @param device 
		/// @param vertexShaderBlob 
		/// @param pixelShaderBlob 
		/// @param log 
		virtual void Initialize(ID3D12Device* device, IDxcBlob* vertexShaderBlob, IDxcBlob* pixelShaderBlob, Log* log) = 0;

		/// @brief ブレンドモードの設定
		/// @param blendMode 
		void SetBlendMode(BlendMode blendMode) { blendMode_ = static_cast<int>(blendMode); }

		/// @brief コマンドリストに登録する
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList) const;

		/// @brief ブレンドモードを初期化する
		virtual void ResetBlendMode() = 0;


		template<class T>
		using ComPtr = Microsoft::WRL::ComPtr<T>;


	protected:


		// シグネチャのバイナリデータ
		ComPtr<ID3DBlob> signatureBlob_ = nullptr;

		// エラーのバイナリデータ
		ComPtr<ID3DBlob> errorBlob_ = nullptr;

		// ルートシグネチャ
		ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;


		// PSO
		ComPtr<ID3D12PipelineState> graphicsPipelineState_[static_cast<int>(BlendMode::kNumBlendMode)];



		// 現在のブレンドモード
		int32_t blendMode_ = static_cast<int>(BlendMode::kNone);
	};
}