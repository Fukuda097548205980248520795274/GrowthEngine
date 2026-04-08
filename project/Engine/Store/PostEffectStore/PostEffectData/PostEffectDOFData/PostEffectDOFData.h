#pragma once
#include "Resource/OffscreenResource/OffscreenResource.h"
#include "../PostEffectBaseData.h"

namespace Engine
{
	class DX12Buffering;
	class DX12Heap;

	class PostEffectDOFData : public PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectDOFData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect, PostEffectParameter* parameter)
			: PostEffectBaseData(name, type, hPostEffect, parameter) {
		}

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, DX12Buffering* buffering, DX12Heap* heap, Log* log, BasePSOPostEffect* pso, BasePSOPostEffect* psoGaussianBlur);

		/// @brief リセット
		void Reset() override;

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
		std::unique_ptr<PostEffect::DOF> param_ = nullptr;

		// リソース
		std::unique_ptr<ConstantBufferResource<PostEffect::DOFDataForGPU>> resource_ = nullptr;

		/// @brief ブラーをかけるためのオフスクリーンリソース
		std::unique_ptr<OffscreenResource> blurTextureResource_ = nullptr;


	private:

		/// @brief ガウシアンフィルターPSO
		BasePSOPostEffect* psoGaussianBlur_ = nullptr;
	};
}