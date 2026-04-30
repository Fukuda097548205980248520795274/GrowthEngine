#pragma once
#include "../PostEffectBaseData.h"

namespace Engine
{
	class PostEffectAfterImageData : public PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectAfterImageData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect, PostEffectParameter* parameter)
			: PostEffectBaseData(name, type, hPostEffect, parameter) {
		}

		/// @brief 初期化
		/// @param device 
		/// @param commandList 
		/// @param heap 
		/// @param buffering 
		/// @param psoAfterImage 
		/// @param log 
		void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, DX12Buffering* buffering, BaseComputePSO* psoAfterImage, Log* log);

		/// @brief リセット
		void Reset() override;

		/// @brief リサイズ
		/// @param device 
		/// @param commandList 
		/// @param width 
		/// @param height 
		void Resize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, int32_t width, int32_t height) override;

		/// @brief コマンドリストに登録する
		/// @param context
		void Register(const PostEffectRenderContext& context) override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	private:

		/// @brief パラメータ
		std::unique_ptr<PostEffect::AfterImage> param_ = nullptr;

		// リソース
		std::unique_ptr<ConstantBufferResource<PostEffect::AfterImageDataForGPU>> resource_ = nullptr;

		// 前フレームの残像用リソース
		std::unique_ptr<OffscreenResource> prevFrameOffscreenResource_ = nullptr;

		// 出力リソース
		std::unique_ptr<RWTexture2DBufferResource> outputResource_ = nullptr;

		/// @brief 前フレームの残像出力リソース
		std::unique_ptr<RWTexture2DBufferResource> prevAfterImageOutputResource_ = nullptr;

		/// @brief AfterImage用のPSO
		BaseComputePSO* psoAfterImage_ = nullptr;
	};
}