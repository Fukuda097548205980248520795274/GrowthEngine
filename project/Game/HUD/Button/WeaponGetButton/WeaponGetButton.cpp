#include "WeaponGetButton.h"

/// @brief コンストラクタ
/// @param initData 
WeaponGetButton::WeaponGetButton(const InitData& initData)
{
	if (initData.buttonSprite)
	{
		buttonSprite_ = initData.buttonSprite;

		buttonSprite_->param_.parent = worldTransform_.get();
	}
}

/// @brief デストラクタ
WeaponGetButton::~WeaponGetButton()
{
	if (buttonSprite_)
	{
		buttonSprite_->isDelete_ = true;
		buttonSprite_ = nullptr;
	}
}

/// @brief 更新処理
void WeaponGetButton::Update()
{
	// 壊れたときの処理
	if (isBreak_)
	{
		fadeOutTimer_ -= engine_->GetDeltaTime();
		fadeOutTimer_ = std::max(fadeOutTimer_, 0.0f);
		t_ = std::clamp(fadeOutTimer_ / kFadeOutDuration, 0.0f, 1.0f);

		// フェードアウトが完了したら終了する
		if (t_ <= 0.0f)
		{
			isFinished_ = true;
		}

		// 基底クラスの更新
		Button::Update();

		return;
	}

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

	// 基底クラスの更新
	Button::Update();
}

/// @brief 描画処理
void WeaponGetButton::Draw()
{
	// 表示されていない場合は描画しない
	if (!isVisible_)return;

	if (t_ <= 0.0f && buttonSprite_)
	{
		buttonSprite_->param_.material.color.w = 0.0f;

		return;
	}

	if (buttonSprite_)
	{
		buttonSprite_->param_.material.color.w = t_;
		buttonSprite_->Draw();
	}
}

/// @brief 壊れたことを通知する
void WeaponGetButton::Break()
{
	// 既に壊れている場合は何もしない
	if (isBreak_)return;

	// フェードアウトの時間をリセットする
	fadeOutTimer_ = kFadeInDuration - fadeInTimer_;

	isBreak_ = true;
}