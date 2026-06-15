#include "Timer.h"

/// @brief デストラクタ
Timer::~Timer()
{
	// タイマースプライトの削除
	for (int i = 0; i < 10; ++i)
	{
		if (timerSprite_[i])
		{
			timerSprite_[i]->isDelete_ = true;
			timerSprite_[i] = nullptr;
		}
	}

	// コンマスプライトの削除
	if (commaSprite_)
	{
		commaSprite_->isDelete_ = true;
		commaSprite_ = nullptr;
	}
}

/// @brief 更新処理
void Timer::Update()
{
	int timeInMilliseconds = static_cast<int>(currentTime_ * 10.0f);

	// 各桁の値を計算して保存する
	for (int i = 0; i < 4; ++i)
	{
		digit_[3 - i] = timeInMilliseconds % 10;
		timeInMilliseconds /= 10;
	}
}

/// @brief 描画処理
void Timer::Draw()
{
	// 桁ごとの時間を描画
	for (int i = 0; i < 4; ++i)
	{
		if (timerSprite_[digit_[i]])timerSprite_[digit_[i]]->Draw();
	}
}