#include "Tutorial.h"

/// @brief 更新処理
void Tutorial::Update()
{
	// 更新が無効な場合は処理しない
	if (!updateEnabled_)return;

	if (state_ == State::Practice)
	{
		// タイマーを減らす
		practiceTimer_ -= engine_->GetDeltaTime();

		// タイマーが0以下になったら状態を切り替える
		if (practiceTimer_ <= 0.0f)
		{
			state_ = State::FadeOut;
		}
	}
	else if (state_ == State::Clear)
	{
		// タイマーを減らす
		clearTimer_ -= engine_->GetDeltaTime();

		// タイマーが0以下になったら状態を切り替える
		if (clearTimer_ <= 0.0f)
		{
			state_ = State::FadeOut;
		}
	}
	else if (state_ == State::FadeOut)
	{
		// フェードアウトタイマーを減らす
		fadeOutTimer_ -= engine_->GetDeltaTime();

		// タイマーが0以下になったら終了する
		if (fadeOutTimer_ <= 0.0f)
		{
			isFinished_ = true;
		}
	}

	// 基底クラスの更新
	HUD::Update();
}

/// @brief 描画処理
void Tutorial::Draw()
{
	
}