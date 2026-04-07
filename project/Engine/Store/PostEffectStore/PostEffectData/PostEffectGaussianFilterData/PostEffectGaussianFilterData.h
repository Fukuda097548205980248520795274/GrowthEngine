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
		/// @param commandList 
		void Register(ID3D12GraphicsCommandList* commandList, OffscreenResource* offscreenResource,
			DepthResource* depthResource, const Matrix4x4& projectionInverse) override;

		/// @brief パラメータを取得する
		/// @return 
		void* GetParam() override { return nullptr; }

		/// @brief デバッグ用パラメータ
		void DebugParameter() override;
	};
}