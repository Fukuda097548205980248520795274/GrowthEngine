#pragma once
#include <string>
#include "Handle/Handle.h"
#include "Data/PostEffectData/PostEffectData.h"

class GrowthEngine;

namespace Engine
{
	class BasePostEffect
	{
	public:

		/// @brief 仮想デストラクタ
		virtual ~BasePostEffect() = default;

		/// @brief コンストラクタ
		/// @param name 
		BasePostEffect(const std::string& name) : name_(name){}

		/// @brief 描画処理
		virtual void Draw() = 0;


	protected:

		// エンジン
		const GrowthEngine* engine_ = nullptr;

		/// @brief 名前
		std::string name_{};

		/// @brief 種類
		Engine::PostEffect::Type type_;

		/// @brief ハンドル
		PostEffectHandle handle_ = 0;
	};
}