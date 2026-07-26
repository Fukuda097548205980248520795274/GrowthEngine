#pragma once
#include "../BasePostEffect.h"

class PostEffectBlurShadow2D : public Engine::BasePostEffect
{
public:

	/// @brief コンストラクタ
	/// @param name 名前
	PostEffectBlurShadow2D(const std::string& name);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::PostEffect::BlurShadow2D* param_ = nullptr;
};

