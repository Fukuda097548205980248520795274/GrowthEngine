#include "CharacterStateAttack.h"
#include "Entity/Character/Character.h"
#include "Action/Attack/Attack.h"

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateAttack::Enter()
{
	// 攻撃を実行する
	if(currentAttack_)
		currentAttack_->Exec();
}

/// @brief 更新処理
/// @param dt 
void CharacterStateAttack::Update(float dt)
{
	// 攻撃を更新する
	if (currentAttack_)
	{
		currentAttack_->Update();

		// 攻撃が消えてしまったら、ここで中断する
		if (!currentAttack_)return;

		if (currentAttack_->IsFinishedTimer())
		{
			// 攻撃が終了したら、状態をNoneに変更する
			owner_->GetStateMachine()->ChangeState("None");
			return;
		}
	}
	else
	{
		// 攻撃がない場合は、状態をNoneに変更する
		owner_->GetStateMachine()->ChangeState("None");
		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateAttack::Exit()
{
	// 攻撃を終了する
	currentAttack_ = nullptr;
}