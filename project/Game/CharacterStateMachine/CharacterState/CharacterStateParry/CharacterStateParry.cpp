#include "CharacterStateParry.h"
#include "Entity/Character/Character.h"

/// @brief コンストラクタ
	/// @param owner 
CharacterStateParry::CharacterStateParry(Character* owner, AnimationHandle hMotion)
	: CharacterState(owner), hMotion_(hMotion)
{
}

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateParry::Enter()
{
	// ビヘイビアツリーのリクエストを行う
	BehaviorTreeRequest();

	// タイマーをリセットする
	actionTimer_ = maxActionTime_;

	// アニメーションを設定する
	owner_->SetAnimation(hMotion_, true, false);
}

/// @brief 更新処理
/// @param dt 
void CharacterStateParry::Update(float dt)
{
	// ダメージタイマーを更新する
	actionTimer_ -= dt;

	// ダメージタイマーが0以下になったら、通常状態へ移行する
	if (actionTimer_ <= 0.0f)
	{
		owner_->GetStateMachine()->ChangeState("None");
		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateParry::Exit()
{
	// タイマーをリセットする
	actionTimer_ = maxActionTime_;
}