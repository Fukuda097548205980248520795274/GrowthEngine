#pragma once
#include "../PostEffectBaseData.h"

namespace Engine
{
	class DX12Buffering;

	class PostEffectTAAData : public PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectTAAData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect, PostEffectParameter* parameter)
			: PostEffectBaseData(name, type, hPostEffect, parameter) {
		}

		/// @brief 初期化
		/// @param device 
		/// @param commandList 
		/// @param heap 
		/// @param buffering 
		/// @param psoTAA 
		/// @param log 
		void Initialize(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, DX12Heap* heap, DX12Buffering* buffering, BaseComputePSO* psoTAA, Log* log);

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
		std::unique_ptr<PostEffect::TAA> param_ = nullptr;

		// リソース
		std::unique_ptr<ConstantBufferResource<PostEffect::TAADataForGPU>> resource_ = nullptr;

		// TAA用の出力リソース
		std::unique_ptr<RWTexture2DBufferResource> outputTAAResource_ = nullptr;

		/// @brief 前フレームのリソース
		std::unique_ptr<OffscreenResource> prevFrameResource_ = nullptr;

		/// @brief TAA用のPSO
		BaseComputePSO* psoTAA_ = nullptr;
	};
}