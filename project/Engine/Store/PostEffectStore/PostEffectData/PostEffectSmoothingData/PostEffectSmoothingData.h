#pragma once
#include "../PostEffectBaseData.h"

namespace Engine
{
	class PostEffectSmoothingData : public PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectSmoothingData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect, PostEffectParameter* parameter)
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
		void* GetParam() override { return nullptr; }

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;
	};
}