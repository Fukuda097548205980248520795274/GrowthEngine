#include "Effect.h"

void Effect::Update()
{
	// 時間を更新する
	duration_ -= engine_->GetDeltaTime() * engine_->GetTimeScale();

	// 終了時間を過ぎたら終了フラグを立てる
	if (duration_ <= 0.0f)
	{
		isFinished_ = true;
	}
}