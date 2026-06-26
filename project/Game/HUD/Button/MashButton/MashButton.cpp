#include "MashButton.h"
#include <numbers>

/// @brief デストラクタ
MashButton::~MashButton()
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
void MashButton::Initialize(const InitData& initData)
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

	// 色を設定
	buttonInSprite_->param_.material.color = Vector4(initData.color.x, initData.color.y, initData.color.z, 1.0f);
	buttonOutSprite_->param_.material.color = Vector4(initData.color.x, initData.color.y, initData.color.z, 1.0f);

	// 位置を設定
	worldTransform_->translate_ = initData.position;
	worldTransform_->scale_ = initData.scale;
	worldTransform_->Update();
}

/// @brief 更新処理
void MashButton::Update()
{
	if (state_ == State::FadeIn)
	{
		fadeInTimer_ -= engine_->GetDeltaTime();
		alphaRate_ = 1.0f - (fadeInTimer_ / kFadeInTime);

		if (fadeInTimer_ <= 0.0f)
		{
			state_ = State::None;
			alphaRate_ = 1.0f;
		}
	} 
	else if (state_ == State::FadeOut)
	{
		fadeOutTimer_ -= engine_->GetDeltaTime();
		alphaRate_ = fadeOutTimer_ / kFadeOutTime;

		if (fadeOutTimer_ <= 0.0f)
		{
			state_ = State::None;
			alphaRate_ = 0.0f;
		}
	}
	else if (state_ == State::Input)
	{
		inputTimer_ -= engine_->GetDeltaTime();
		float scaleRate = std::clamp(1.0f - (inputTimer_ / kInputTime), 0.0f, 1.0f);
		scaleRate = 1.0f - std::powf(1.0f - scaleRate, 2.0f);

		float scale = Lerp(kInputOutScale, 1.0f, scaleRate);
		buttonOutSprite_->param_.transform.scale = Vector2(scale, scale);

		if (inputTimer_ <= 0.0f)
		{
			state_ = State::None;
			buttonOutSprite_->param_.transform.scale = Vector2(1.0f, 1.0f);
			outScaleParameter_ = 0.0f;
		}
	}
	else if (state_ == State::None)
	{
		outScaleParameter_ += std::clamp(engine_->GetDeltaTime() * kOutScaleParameterSpeed , 0.0f, std::numbers::pi_v<float>);
		float scale = 1.0f + std::sin(outScaleParameter_) * 0.0125f;
		outAlphaRate_ = 1.0f - std::sin(std::fabs(outScaleParameter_)) * 0.5f;

		buttonOutSprite_->param_.transform.scale = Vector2(scale, scale);
	}

	// α値を設定
	buttonSprite_->param_.material.color.w = alphaRate_;
	buttonInSprite_->param_.material.color.w = alphaRate_;
	buttonOutSprite_->param_.material.color.w = alphaRate_ * outAlphaRate_;

	Button::Update();
}

/// @brief 描画処理
void MashButton::Draw()
{
	// α値が0以下の場合は描画しない
	if (alphaRate_ <= 0.0f)return;

	buttonSprite_->Draw();
	buttonInSprite_->Draw();
	buttonOutSprite_->Draw();
}

/// @brief 入力処理
void  MashButton::Input()
{
	state_ = State::Input;
	inputTimer_ = kInputTime;
	buttonOutSprite_->param_.transform.scale = Vector2(kInputOutScale, kInputOutScale);

	// 外側のスプライトのα値をリセット
	outAlphaRate_ = 1.0f;
}

/// @brief フェードインを開始する
void MashButton::FadeIn()
{
	state_ = State::FadeIn;
	fadeInTimer_ = kFadeInTime;

	// 外側のスプライトのα値をリセット
	outAlphaRate_ = 1.0f;
}

/// @brief フェードアウトを開始する
void MashButton::FadeOut()
{
	state_ = State::FadeOut;
	fadeOutTimer_ = kFadeOutTime;

	// 外側のスプライトのα値をリセット
	outAlphaRate_ = 1.0f;
}