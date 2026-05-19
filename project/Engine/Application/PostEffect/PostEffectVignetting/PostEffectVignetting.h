#pragma once
#include "../BasePostEffect.h"

class GrowthEngine;

class PostEffectVignetting : public Engine::BasePostEffect
{
public:

	/// @brief コンストラクタ
	/// @param name 名前
	PostEffectVignetting(const std::string& name);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::PostEffect::Vignetting* param_ = nullptr;
};

