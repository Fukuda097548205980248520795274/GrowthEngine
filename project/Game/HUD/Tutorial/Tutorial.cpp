#include "Tutorial.h"

/// @brief 更新処理
void Tutorial::Update()
{
	// 有効化した瞬間
	if (!prevEnable_ && enable_)
	{
		fadeInTimer_ = kFadeOutDuration - fadeOutTimer_;
	}
	else if (prevEnable_ && !enable_)
	{
		fadeOutTimer_ = kFadeInDuration - fadeInTimer_;
	}

	// 更新が無効なら何もしない
	prevEnable_ = enable_;

	if (enable_)
	{
		fadeInTimer_ -= engine_->GetDeltaTime();
		fadeInTimer_ = std::max(fadeInTimer_, 0.0f);
		t_ = 1.0f - std::clamp(fadeInTimer_ / kFadeInDuration, 0.0f, 1.0f);
	}
	else
	{
		fadeOutTimer_ -= engine_->GetDeltaTime();
		fadeOutTimer_ = std::max(fadeOutTimer_, 0.0f);
		t_ = std::clamp(fadeOutTimer_ / kFadeOutDuration, 0.0f, 1.0f);
	}

	// 有効化をリセットする
	enable_ = false;

	// スプライトの更新
	HUD::Update();
}

/// @brief 描画処理
void Tutorial::Draw()
{
	// 透明度が0以下なら描画しない
	if (t_ <= 0.0f)return;

	for (auto& sprite : sprites_)
	{
		sprite->param_->material.color.w = t_;
		sprite->Draw();
	}
}