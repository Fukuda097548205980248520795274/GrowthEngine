#pragma once
#include "../PostEffectBaseData.h"

namespace Engine
{
	class Log;
	class BasePSOPostEffect;
	class OffscreenResource;

	class PostEffectRadialBlurData : public PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectRadialBlurData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect)
			: PostEffectBaseData(name,type,hPostEffect){ }

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso);

		/// @brief コマンドリストに登録する
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList, OffscreenResource* offscreenResource) override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }


	private:

		/// @brief PSO
		BasePSOPostEffect* pso_ = nullptr;


	private:

		/// @brief パラメータ
		std::unique_ptr<PostEffect::RadialBlur> param_ = nullptr;

		// リソース
		std::unique_ptr<ConstantBufferResource<PostEffect::RadialBlurDataForGPU>> resource_ = nullptr;
	};
}