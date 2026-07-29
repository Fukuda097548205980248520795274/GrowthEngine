#pragma once
#include "../PostEffectBaseData.h"

namespace Engine
{
	class PostEffectGaussianFilterData : public PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectGaussianFilterData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect, PostEffectParameter* parameter)
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

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	private:

		/// @brief パラメータ
		std::unique_ptr<PostEffect::GaussianFilter> param_ = nullptr;

		// @brief 水平方向のガウスフィルタ用の定数バッファリソース
		std::unique_ptr<ConstantBufferResource<PostEffect::GaussianFilterDataForGPU>> horizontalResource_ = nullptr;

		// @brief 垂直方向のガウスフィルタ用の定数バッファリソース
		std::unique_ptr<ConstantBufferResource<PostEffect::GaussianFilterDataForGPU>> verticalResource_ = nullptr;
	};
}