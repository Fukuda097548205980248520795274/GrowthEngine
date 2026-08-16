#include "IntroText.h"

/// @brief デストラクタ
IntroText::~IntroText()
{
	// 親を解除
	buttonSprite_->SetParent(nullptr);
}

/// @brief 初期化
/// @param initData 
void IntroText::Initialize(const InitData& initData)
{
	assert(initData.buttonSprite && "ボタンスプライトが設定されていません。");

	// ボタンのスプライトを設定
	buttonSprite_ = initData.buttonSprite;

	// 親を設定
	buttonSprite_->SetParent(worldTransform_.get());

	// 位置を設定
	worldTransform_->Update();
}

/// @brief 更新処理
void IntroText::Update()
{
	if (state_ == State::FadeIn)
	{
		fadeInTimer_ -= engine_->GetDeltaTime();
		fadeInTimer_ = std::max(fadeInTimer_, 0.0f);
		alphaRate_ = 1.0f - (fadeInTimer_ / kFadeInTime);
		buttonSprite_->param_->transform.scale = Lerp(Vector2(3.0f, 3.0f), Vector2(1.0f, 1.0f), alphaRate_);

		if (fadeInTimer_ <= 0.0f)
		{
			state_ = State::Stop;
			stopTimer_ = kStopTime;
		}
	}
	else if (state_ == State::Stop)
	{
		stopTimer_ -= engine_->GetDeltaTime();
		stopTimer_ = std::max(stopTimer_, 0.0f);

		if (stopTimer_ <= 0.0f)
		{
			state_ = State::FadeOut;
			fadeOutTimer_ = kFadeOutTime;
		}
	}
	else if (state_ == State::FadeOut)
	{
		fadeOutTimer_ -= engine_->GetDeltaTime();
		fadeOutTimer_ = std::max(fadeOutTimer_, 0.0f);

		alphaRate_ = fadeOutTimer_ / kFadeOutTime;
		buttonSprite_->param_->transform.scale = Lerp(Vector2(1.0f, 1.0f), Vector2(1.3f, 1.3f), 1.0f - alphaRate_);

		if (fadeOutTimer_ <= 0.0f)
		{
			Delete();
		}
	}

	// α値を設定
	buttonSprite_->param_->material.color.w = alphaRate_;

	HUD::Update();
}

/// @brief 描画処理
void IntroText::Draw()
{
	// α値が0以下の場合は描画しない
	if (alphaRate_ <= 0.0f)return;

	buttonSprite_->Draw();
}