#pragma once
#include "../PostEffectBaseData.h"

namespace Engine
{
	class TextureStore;

	class PostEffectDissolveData : public PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectDissolveData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect, PostEffectParameter* parameter)
			: PostEffectBaseData(name, type, hPostEffect, parameter) {
		}

		/// @brief 初期化
		/// @param device 
		/// @param log 
		void Initialize(ID3D12Device* device, Log* log, BasePSOPostEffect* pso, TextureStore* textureStore);

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
		std::unique_ptr<PostEffect::Dissolve> param_ = nullptr;

		// リソース
		std::unique_ptr<ConstantBufferResource<PostEffect::DissolveDataForGPU>> resource_ = nullptr;

		/// @brief マスクテクスチャのファイルパス
		std::string maskTextureFilePath_;

		/// @brief テクスチャストア
		TextureStore* textureStore_ = nullptr;
	};
}