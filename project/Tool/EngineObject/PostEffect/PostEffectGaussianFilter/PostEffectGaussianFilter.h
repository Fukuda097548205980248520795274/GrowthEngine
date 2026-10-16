#pragma once
#include "../BasePostEffect.h"

/// @brief ガウスブラー
class PostEffectGaussianFilter : public Engine::BasePostEffect
{
public:

	/// @brief コンストラクタ
	/// @param name 名前
	PostEffectGaussianFilter(const std::string& name);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::PostEffect::GaussianFilter* param_ = nullptr;
};

