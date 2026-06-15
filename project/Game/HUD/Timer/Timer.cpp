#include "Timer.h"

/// @brief デストラクタ
Timer::~Timer()
{
	// タイマースプライトの削除
	for (int i = 0; i < 4; ++i)
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

/// @brief 初期化
/// @param initData 
void Timer::Initialize(const InitData& initData)
{
	for (int i = 0; i < 4; ++i)
	{
		timerSprite_[i] = initData.timerSprite[i];
	}
	commaSprite_ = initData.commaSprite;
	currentTime_ = initData.currentTime;
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
	
}