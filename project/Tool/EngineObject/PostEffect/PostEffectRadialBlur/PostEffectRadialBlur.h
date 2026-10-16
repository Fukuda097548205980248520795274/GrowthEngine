#pragma once
#include "../BasePostEffect.h"

/// @brief エンジン
class GrowthEngine;

/// @brief ラジアルブラー
class PostEffectRadialBlur : public Engine::BasePostEffect
{
public:

	/// @brief コンストラクタ
	/// @param name 名前
	PostEffectRadialBlur(const std::string& name);

	/// @brief 描画処理
	void Draw() override;

	/// @brief パラメータ
	Engine::PostEffect::RadialBlur* param_ = nullptr;
};

