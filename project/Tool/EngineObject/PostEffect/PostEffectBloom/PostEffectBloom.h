#pragma once
#include "../BasePostEffect.h"

class PostEffectBloom : public Engine::BasePostEffect
{
public:

	/// @brief コンストラクタ
	/// @param name 名前
	PostEffectBloom(const std::string& name);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::PostEffect::Bloom* param_ = nullptr;
};

