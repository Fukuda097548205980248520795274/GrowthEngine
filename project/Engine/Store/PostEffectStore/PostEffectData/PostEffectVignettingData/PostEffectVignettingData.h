#pragma once
#include "../PostEffectBaseData.h"

namespace Engine
{
	class Log;

	class PostEffectVignettingData : public PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectVignettingData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect, PostEffectParameter* parameter)
			: PostEffectBaseData(name, type, hPostEffect, parameter) {
		}

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso);

		/// @brief リセット
		void Reset() override;

		/// @brief コマンドリストに登録する
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList, OffscreenResource* offscreenResource) override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return param_.get(); }

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;


	private:

		/// @brief パラメータ
		std::unique_ptr<PostEffect::Vignetting> param_ = nullptr;

		// リソース
		std::unique_ptr<ConstantBufferResource<PostEffect::VignettingDataForGPU>> resource_ = nullptr;
	};
}