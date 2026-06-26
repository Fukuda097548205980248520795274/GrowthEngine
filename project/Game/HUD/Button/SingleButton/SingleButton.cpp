#include "SingleButton.h"

/// @brief デストラクタ
SingleButton::~SingleButton()
{
	// ボタンのスプライトを削除
	if (buttonSprite_)buttonSprite_->isDelete_ = true;
	buttonSprite_ = nullptr;

	// ボタンの内側スプライトを削除
	if (buttonInSprite_)buttonInSprite_->isDelete_ = true;
	buttonInSprite_ = nullptr;

	// ボタンの外側スプライトを削除
	if (buttonOutSprite_)buttonOutSprite_->isDelete_ = true;
	buttonOutSprite_ = nullptr;
}

/// @brief 初期化
/// @param initData 
void SingleButton::Initialize(const InitData& initData)
{
	assert(initData.buttonSprite && "ボタンスプライトが設定されていません。");
	assert(initData.buttonInSprite && "ボタン内側スプライトが設定されていません。");
	assert(initData.buttonOutSprite && "ボタン外側スプライトが設定されていません。");

	// ボタンのスプライトを設定
	buttonSprite_ = initData.buttonSprite;
	buttonInSprite_ = initData.buttonInSprite;
	buttonOutSprite_ = initData.buttonOutSprite;

	// 親を設定
	buttonSprite_->param_.parent = worldTransform_.get();
	buttonInSprite_->param_.parent = worldTransform_.get();
	buttonOutSprite_->param_.parent = worldTransform_.get();

	alphaRate_ = 1.0f;

	// 位置を設定
	worldTransform_->translate_ = initData.position;
	worldTransform_->Update();
}

/// @brief 更新処理
void SingleButton::Update()
{
	if (state_ == State::FadeIn)
	{
		fadeInTimer_ -= engine_->GetDeltaTime();
		alphaRate_ = 1.0f - (fadeInTimer_ / kFadeInTime);

		if (fadeInTimer_ <= 0.0f)
		{
			state_ = State::OutScaling;
			inScalingTimer_ = kOutScalingTime;
		}
	} 
	else if (state_ == State::OutScaling)
	{
		inScalingTimer_ -= engine_->GetDeltaTime();
		float scaleRate = inScalingTimer_ / kOutScalingTime;

		buttonOutSprite_->param_.transform.scale = Vector2(1.0f + (1.0f - scaleRate), 1.0f + (1.0f - scaleRate));
		if (inScalingTimer_ <= 0.0f)
		{
			state_ = State::None;
			buttonOutSprite_->param_.transform.scale = Vector2(1.0f, 1.0f);
		}
	} 
	else if (state_ == State::FadeOut)
	{
		fadeOutTimer_ -= engine_->GetDeltaTime();
		alphaRate_ = fadeOutTimer_ / kFadeOutTime;
	}

	// α値を設定
	buttonSprite_->param_.material.color.w = alphaRate_;
	buttonInSprite_->param_.material.color.w = alphaRate_;
	buttonOutSprite_->param_.material.color.w = alphaRate_;

	Button::Update();
}

/// @brief 描画処理
void SingleButton::Draw()
{
	buttonSprite_->Draw();
	buttonInSprite_->Draw();
	buttonOutSprite_->Draw();
}

/// @brief 入力処理
void  SingleButton::Input()
{
	// 入力したらフェードアウトする
	FadeOut();
}

/// @brief フェードインを開始する
void SingleButton::FadeIn()
{
	state_ = State::FadeIn;
	fadeInTimer_ = kFadeInTime;
}

/// @brief フェードアウトを開始する
void SingleButton::FadeOut()
{
	state_ = State::FadeOut;
	fadeOutTimer_ = kFadeOutTime;
}