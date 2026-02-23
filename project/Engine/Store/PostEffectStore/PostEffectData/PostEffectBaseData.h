#pragma once
#include <string>
#include <memory>
#include "Data/PostEffectData/PostEffectData.h"
#include "DataForGPU/PostEffectDataForGPU/PostEffectDataForGPU.h"
#include "Resource/ConstantBufferResource/ConstantBufferResource.h"
#include "Handle/Handle.h"

namespace Engine
{
	class OffscreenResource;
	class BasePSOPostEffect;

	class PostEffectBaseData
	{
	public:

		/// @brief コンストラクタ
		/// @param name 
		/// @param type 
		/// @param hPostEffect 
		PostEffectBaseData(const std::string name, PostEffect::Type type, PostEffectHandle hPostEffect)
			: name_(name), type_(type), hPostEffect_(hPostEffect) {
		}

		/// @brief 仮想デストラクタ
		virtual ~PostEffectBaseData() = default;

		/// @brief 種類を取得する
		/// @return 
		PostEffect::Type GetType()const { return type_; }

		/// @brief 名前を取得する
		/// @return 
		std::string GetName()const { return name_; }

		/// @brief ハンドルを取得する
		/// @return 
		PostEffectHandle GetHandle()const { return hPostEffect_; }

		/// @brief パラメータを取得する
		/// @return 
		virtual void* GetParam() = 0;

		/// @brief コマンドリストに登録する
		/// @param commandList 
		virtual void Register(ID3D12GraphicsCommandList* commandList, OffscreenResource* offscreenResource) = 0;


	private:

		// 名前
		std::string name_{};

		/// @brief 種類
		PostEffect::Type type_;

		// ハンドル
		PostEffectHandle hPostEffect_ = 0;


	protected:

		/// @brief PSO
		BasePSOPostEffect* pso_ = nullptr;
	};
}