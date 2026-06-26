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

	// 位置を設定
	worldTransform_->translate_ = initData.position;
	worldTransform_->scale_ = initData.scale;
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
			state_ = State::None;
			inScalingTimer_ = kOutScalingTime;
		}
	} 
	else if (state_ == State::Input)
	{
		inputTimer_ -= engine_->GetDeltaTime();

		float scaleRate = std::clamp(1.0f - (inputTimer_ / kInputTime), 0.0f, 1.0f);
		scaleRate = 1.0f - std::powf(1.0f - scaleRate, 2.0f);

		// 外側のスプライトのスケールを設定
		float scale = Lerp(0.0f, kInputOutScale, scaleRate);
		buttonOutSprite_->param_.transform.scale = Vector2(scale, scale);

		// α値を設定
		outAlphaRate_ = 1.0f - scaleRate;

		if (inputTimer_ <= 0.0f)
		{
			state_ = State::FadeOut;
			buttonOutSprite_->param_.transform.scale = Vector2(kInputOutScale, kInputOutScale);
			outAlphaRate_ = 0.0f;
		}
	}
	else if (state_ == State::FadeOut)
	{
		fadeOutTimer_ -= engine_->GetDeltaTime();

		float scaleRate = std::clamp(1.0f - (fadeOutTimer_ / kFadeOutTime), 0.0f, 1.0f);
		scaleRate = std::powf(scaleRate, 2.0f);

		// 外側のスプライトのスケールを設定
		float scale = Lerp(kOutScale, 0.0f, scaleRate);
		buttonOutSprite_->param_.transform.scale = Vector2(scale, scale);

		// α値を設定
		alphaRate_ = fadeOutTimer_ / kFadeOutTime;

		if (fadeOutTimer_ <= 0.0f)
		{
			state_ = State::None;
			buttonOutSprite_->param_.transform.scale = Vector2(0.0f, 0.0f);
		}
	}
	else if (state_ == State::None)
	{
		inScalingTimer_ -= engine_->GetDeltaTime();

		float scaleRate = std::clamp(1.0f - (inScalingTimer_ / kOutScalingTime), 0.0f, 1.0f);
		scaleRate = 1.0f - std::powf(1.0f - scaleRate, 2.0f);

		// 外側のスプライトのスケールを設定
		float scale = Lerp(0.0f, kOutScale, scaleRate);
		buttonOutSprite_->param_.transform.scale = Vector2(scale, scale);

		// α値を設定
		outAlphaRate_ = 1.0f - scaleRate;

		// タイマーが0以下になったらリセット
		if (inScalingTimer_ <= 0.0f)inScalingTimer_ = kOutScalingTime;
	}

	// α値を設定
	buttonSprite_->param_.material.color.w = alphaRate_;
	buttonInSprite_->param_.material.color.w = alphaRate_;
	buttonOutSprite_->param_.material.color.w = alphaRate_ * outAlphaRate_;

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
	state_ = State::Input;
	inputTimer_ = kInputTime;
	buttonOutSprite_->param_.transform.scale = Vector2(kInputOutScale, kInputOutScale);
	outAlphaRate_ = 1.0f;
}

/// @brief フェードインを開始する
void SingleButton::FadeIn()
{
	state_ = State::FadeIn;
	fadeInTimer_ = kFadeInTime;
	buttonOutSprite_->param_.transform.scale = Vector2(0.0f, 0.0f);
}

/// @brief フェードアウトを開始する
void SingleButton::FadeOut()
{
	state_ = State::FadeOut;
	fadeOutTimer_ = kFadeOutTime;
}