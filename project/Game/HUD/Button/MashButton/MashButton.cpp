#include "MashButton.h"

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

	// 位置を設定
	worldTransform_->translate_ = initData.position;
	worldTransform_->Update();
}

/// @brief 更新処理
void MashButton::Update()
{
	if (state_ == State::FadeIn)
	{
		fadeInTimer_ -= engine_->GetDeltaTime();
		alphaRate_ = 1.0f - (fadeInTimer_ / kFadeInTime);
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
void MashButton::Draw()
{
	buttonSprite_->Draw();
	buttonInSprite_->Draw();
	buttonOutSprite_->Draw();
}

/// @brief 入力処理
void  MashButton::Input()
{
	
}

/// @brief フェードインを開始する
void MashButton::FadeIn()
{
	state_ = State::FadeIn;
	fadeInTimer_ = kFadeInTime;
}

/// @brief フェードアウトを開始する
void MashButton::FadeOut()
{
	state_ = State::FadeOut;
	fadeOutTimer_ = kFadeOutTime;
}