#include "CharacterStateStance.h"
#include "Entity/Character/Player/Player.h"
#include "Entity/Character/NPC/NPC.h"

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateStance::Enter()
{
	// 構えモーションの再生を開始する
	owner_->SetAnimation(hStanceMotion_, true, true);
}

/// @brief 更新処理
/// @param dt 
void CharacterStateStance::Update(float dt)
{
	// ロックオン対象がいない場合は、None状態に遷移する
	if (!owner_->GetLockOnTarget())
		owner_->GetStateMachine()->ChangeState("None");
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateStance::Exit()
{

}