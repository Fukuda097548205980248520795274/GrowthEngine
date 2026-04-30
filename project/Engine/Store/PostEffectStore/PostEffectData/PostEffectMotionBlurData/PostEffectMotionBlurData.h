#pragma once
#include "../PostEffectBaseData.h"

namespace Engine
{
	class PostEffectMotionBlurData : public PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectMotionBlurData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect, PostEffectParameter* parameter)
			: PostEffectBaseData(name, type, hPostEffect, parameter) {
		}

		/// @brief 初期化
		/// @param device 
		/// @param commandList 
		/// @param heap 
		/// @param buffering 
		/// @param psoMotionBlur 
		/// @param log 
		void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, DX12Buffering* buffering, 
			BaseComputePSO* psoMotionBlur,BaseComputePSO* psoVelocityDilation, Log* log);

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
		std::unique_ptr<PostEffect::MotionBlur> param_ = nullptr;

		// リソース
		std::unique_ptr<ConstantBufferResource<PostEffect::MotionBlurDataForGPU>> resource_ = nullptr;

		/// @brief 速度膨張用のリソース
		std::unique_ptr<RWTexture2DBufferResource> velocityDilationResource_ = nullptr;

		// 出力リソース
		std::unique_ptr<RWTexture2DBufferResource> outputResource_ = nullptr;

		/// @brief モーションブラー用のPSO
		BaseComputePSO* psoMotionBlur_ = nullptr;

		/// @brief 速度膨張用のPSO
		BaseComputePSO* psoVelocityDilation_ = nullptr;
	};
}