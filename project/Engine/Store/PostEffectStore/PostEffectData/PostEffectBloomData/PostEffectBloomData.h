#pragma once
#include "Resource/RWTexture2DBufferResource/RWTexture2DBufferResource.h"
#include "../PostEffectBaseData.h"

namespace Engine
{
	class DX12Buffering;
	class DX12Heap;

	class PostEffectBloomData : public PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectBloomData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect, PostEffectParameter* parameter)
			: PostEffectBaseData(name, type, hPostEffect, parameter) {
		}

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Buffering* buffering, DX12Heap* heap,
			BaseComputePSO* highLuminanceExtractionPSO, BaseComputePSO* upsamplePSO, BaseComputePSO* downsamplePSO, Log* log);

		/// @brief リセット
		void Reset() override;

		/// @brief リサイズ
		/// @param width 
		/// @param height 
		void Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int32_t width, int32_t height) override;

		/// @brief コマンドリストに登録する
		/// @param context
		void Register(const PostEffectRenderContext& context) override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief 必要な入力データの種類を取得する
		/// @return
		/// @details 深度テクスチャと逆射影行列が必須
		PostEffectInput GetRequiredInputs() const override
		{
			return PostEffectInput::DepthTexture;
		}

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	private:

		/// @brief パラメータ
		std::unique_ptr<PostEffect::Bloom> param_ = nullptr;

		// リソース
		std::unique_ptr<ConstantBufferResource<PostEffect::HighLuminanceExtractionDataForGPU>> resource_ = nullptr;

		/// @brief デュアルブラー用テクスチャリソース
		std::vector<std::unique_ptr<RWTexture2DBufferResource>> dualBlurTextureResources_;

		// デュアルブラー用のテクスチャの幅と高さ
		int32_t width_ = 0;
		int32_t height_ = 0;

		// デュアルブラーのレベル数
		int32_t numDualBlurLevels_ = 0;


	private:

		// CSデュアルブラー縮小PSO
		BaseComputePSO* downsamplePSO_ = nullptr;

		// CSデュアルブラー拡大PSO
		BaseComputePSO* upsamplePSO_ = nullptr;

		// CS高輝度抽出PSO
		BaseComputePSO* highLuminanceExtractionPSO_ = nullptr;
	};
}