#include "CharacterStateDownGettingUp.h"
#include "Entity/Character/Character.h"

/// @brief コンストラクタ
	/// @param owner 
CharacterStateDownGettingUp::CharacterStateDownGettingUp(Character* owner, AnimationHandle hFront, AnimationHandle hBack)
	: CharacterState(owner), hFront_(hFront), hBack_(hBack)
{
}

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateDownGettingUp::Enter()
{
	// ツリーのリクエストを行う
	TreeRequest();

	// ダメージタイマーをリセットする
	damageTimer_ = maxDamageTime_;
}

/// @brief 更新処理
/// @param dt 
void CharacterStateDownGettingUp::Update(float dt)
{
	/// ツリーのリクエストを行う
	HandleBehaviorTreeNotSet();

	// ダメージタイマーを更新する
	damageTimer_ -= dt;

	// ダメージタイマーが0以下になったら、通常状態へ移行する
	if (damageTimer_ <= 0.0f)
	{
		auto stateMachine = owner_->GetStateMachine();
		stateMachine->ChangeState("None");

		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateDownGettingUp::Exit()
{
	// ダメージリアクションをリセットする
	reaction_ = DamageReactionType::None;
}

/// @brief ダメージリアクションを起こす
/// @param hitPosition 
/// @param attacker 
void CharacterStateDownGettingUp::DamageReaction(DamageReactionType reaction)
{
	// ダメージリアクションを設定する
	reaction_ = reaction;
	if (reaction_ == DamageReactionType::Front)
	{
		// 前向きのアニメーションを再生する
		owner_->SetAnimation(hFront_, true, false);
	}
	else if (reaction_ == DamageReactionType::Back)
	{
		// 後ろ向きのアニメーションを再生する
		owner_->SetAnimation(hBack_, true, false);
	}
}