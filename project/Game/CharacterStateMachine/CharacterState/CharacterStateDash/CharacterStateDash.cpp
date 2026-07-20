#include "CharacterStateDash.h"
#include "Entity/Character/Player/Player.h"
#include "Entity/Character/NPC/NPC.h"

#include "CharacterComponent/CharacterMovement/CharacterMovement.h"

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateDash::Enter()
{
	// ツリーのリクエストを行う
	TreeRequest();

	// ダッシュモーションを再生
	owner_->SetAnimation(hMotion_, false, true);

	// ダッシュ中フラグをリセット
	isDash_ = false;

}

/// @brief 更新処理
/// @param dt 
void CharacterStateDash::Update(float dt)
{
	// ダッシュ中に攻撃状態になった場合は、ダッシュ状態を解除する
	if (owner_->IsAttack())
	{
		auto stateMachine = owner_->GetStateMachine();
		stateMachine->ChangeState("None");
		return;
	}

	// ダッシュ中に回避状態になった場合は、ダッシュ状態を解除する
	auto movement = owner_->GetMovement();

	if (owner_->IsPlayer())
	{
		Player* player = static_cast<Player*>(owner_);
		auto inputController = player->GetInputController();

		// 移動入力があるかどうかを取得する
		bool isMoveInput = false;
		inputController->GetMoveDirection(isMoveInput);

		// ダッシュ入力がない、または移動入力がない場合は、ダッシュ状態を解除する
		if (!isMoveInput)
		{
			auto stateMachine = player->GetStateMachine();
			stateMachine->ChangeState("None");
			return;
		}
	}
	else
	{
		// NPCの場合は、現在の移動がない場合は、ダッシュ状態を解除する
		auto currentMove = owner_->GetCurrentMove();
		if (!currentMove)
		{
			auto stateMachine = owner_->GetStateMachine();
			stateMachine->ChangeState("None");
			return;
		}
	}
	
	// ダッシュ中に移動速度が0以下になった場合は、ダッシュ状態を解除する
	if (isDash_ && movement->GetCurrentVelocity().Length() <= 0.0f)
	{
		auto stateMachine = owner_->GetStateMachine();
		stateMachine->ChangeState("None");
		return;
	}

	// 1フレーム目の更新時にダッシュ中フラグを立てる
	if(!isDash_ && movement->GetCurrentVelocity().Length() > 0.0f)isDash_ = true;
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateDash::Exit()
{
	// ダッシュ中フラグをリセット
	isDash_ = false;
}