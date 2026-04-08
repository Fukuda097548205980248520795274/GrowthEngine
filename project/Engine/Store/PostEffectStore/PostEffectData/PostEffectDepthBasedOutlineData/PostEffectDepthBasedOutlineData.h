#pragma once
#include "../PostEffectBaseData.h"

namespace Engine
{
	class PostEffectDepthBasedOutlineData : public PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectDepthBasedOutlineData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect, PostEffectParameter* parameter)
			: PostEffectBaseData(name, type, hPostEffect, parameter) {
		}

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso);

		/// @brief リセット
		void Reset() override;

		/// @brief コマンドリストに登録する
		/// @param context
		void Register(const PostEffectRenderContext& context) override;

		/// @brief 必要な入力データの種類を取得する
		/// @return
		/// @details 深度テクスチャと逆射影行列が必須
		PostEffectInput GetRequiredInputs() const override
		{
			return PostEffectInput::DepthTexture | PostEffectInput::ProjectionInverse;
		}

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return nullptr; }

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	private:

		// リソース
		std::unique_ptr<ConstantBufferResource<PostEffect::DepthBasedOutlineDataForGPU>> resource_ = nullptr;
	};
}