#include "Defense.h"
#include "Entity/Character/Character.h"
#include "CharacterStateMachine/CharacterState/CharacterStateGuard/CharacterStateGuard.h"

void Defense::Exec()
{
    // ブレークポイントのチェック
	BreakpointOnExec();

    Action::Exec();
    
	// ガード状態に変更する
	owner_->GetStateMachine()->ChangeState("Guard");
	if (auto state = static_cast<CharacterStateGuard*>(owner_->GetStateMachine()->GetCurrentState()))
	{
		state->SetGuardDuration(guardDuration_);

		// 成功
		Update();
		return;
	}

	// 失敗
	Exit();
}

void Defense::Exit()
{
	// ブレークポイントのチェック
	BreakpointOnExit();

    Action::Exit();
}