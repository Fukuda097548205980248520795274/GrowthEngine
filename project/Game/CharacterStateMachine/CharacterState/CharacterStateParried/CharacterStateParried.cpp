#include "CharacterStateParried.h"
#include "Entity/Character/Character.h"

/// @brief コンストラクタ
	/// @param owner 
CharacterStateParried::CharacterStateParried(Character* owner, AnimationHandle hMotion)
	: CharacterState(owner), hMotion_(hMotion)
{
}

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateParried::Enter()
{
	// タイマーをリセットする
	damageTimer_ = maxDamageTime_;

	// アニメーションを設定する
	owner_->SetAnimation(hMotion_, true, false);
}

/// @brief 更新処理
/// @param dt 
void CharacterStateParried::Update(float dt)
{
	// ダメージタイマーを更新する
	damageTimer_ -= dt;

	// ダメージタイマーが0以下になったら、通常状態へ移行する
	if (damageTimer_ <= 0.0f)
	{
		owner_->GetStateMachine()->ChangeState("None");
		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateParried::Exit()
{
	// タイマーをリセットする
	damageTimer_ = maxDamageTime_;
}