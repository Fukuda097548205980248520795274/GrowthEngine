#include "Defense.h"
#include "Entity/Character/Character.h"

void Defense::Exec()
{
    // ブレークポイントのチェック
	BreakpointOnExec();

    Action::Exec();
    owner_->SetGuard(true); // キャラクターを防御状態にする
    guardTimer_ = 0.0f;
}

void Defense::Update()
{
	// ブレークポイントのチェック
	BreakpointOnUpdate();

    guardTimer_ += engine_->GetDeltaTime() * engine_->GetTimeScale();

    // 一定時間ガードしたら終了
    if (guardTimer_ >= guardDuration_)
    {
        owner_->SetGuard(false);
        Action::Update(); // 成功フラグを立ててExit()を呼ぶ
    }
}

void Defense::Exit()
{
	// ブレークポイントのチェック
	BreakpointOnExit();

    owner_->SetGuard(false); // 念のため解除
    Action::Exit();
}