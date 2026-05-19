#pragma once
#include "../BasePostEffect.h"

class GrowthEngine;

class PostEffectGrayscale : public Engine::BasePostEffect
{
public:

	/// @brief コンストラクタ
	/// @param name 名前
	PostEffectGrayscale(const std::string& name);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::PostEffect::Grayscale* param_ = nullptr;
};

