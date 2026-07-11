#include "CharacterStateDownLying.h"
#include "Entity/Character/Character.h"

#include "../CharacterStateDownGettingUp/CharacterStateDownGettingUp.h"
#include "../CharacterStateDownStagger/CharacterStateDownStagger.h"

/// @brief コンストラクタ
	/// @param owner 
CharacterStateDownLying::CharacterStateDownLying(Character* owner, AnimationHandle hFront, AnimationHandle hBack)
	: CharacterState(owner), hFront_(hFront), hBack_(hBack)
{
}

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateDownLying::Enter()
{
	// プレイヤーはタイマーなしで起き上がれる
	if (owner_->IsPlayer())
	{
		damageTimer_ = 0.0f;
	}
	else
	{
		// ダメージタイマーをリセットする
		damageTimer_ = maxDamageTime_;
	}
}

/// @brief 更新処理
/// @param dt 
void CharacterStateDownLying::Update(float dt)
{
	// ダメージタイマーを更新する
	damageTimer_ -= dt;

	// ダウン中にダメージを受けた場合は、ダウン怯み状態へ移行する
	if (owner_->IsHitDamage())
	{
		auto stateMachine = owner_->GetStateMachine();
		stateMachine->ChangeState("DownStagger");
		if (auto downStaggerState = dynamic_cast<CharacterStateDownStagger*>(stateMachine->GetCurrentState()))
		{
			// ダメージリアクションを設定する
			if (reaction_ == DownLyingDamageReaction::Front)
			{
				downStaggerState->DamageReaction(CharacterStateDownStagger::DownStaggerDamageReaction::Front);
			}
			else if (reaction_ == DownLyingDamageReaction::Back)
			{
				downStaggerState->DamageReaction(CharacterStateDownStagger::DownStaggerDamageReaction::Back);
			}
		}

		return;
	}

	// ダメージタイマーが0以下になったら、ダウン状態へ移行する
	if (damageTimer_ <= 0.0f)
	{
		auto stateMachine = owner_->GetStateMachine();
		stateMachine->ChangeState("DownGettingUp");
		if (auto downGettingUpState = dynamic_cast<CharacterStateDownGettingUp*>(stateMachine->GetCurrentState()))
		{
			// ダメージリアクションの方向を設定する
			if (reaction_ == DownLyingDamageReaction::Front)
			{
				downGettingUpState->DamageReaction(CharacterStateDownGettingUp::DownLyingDamageReaction::Front);
			}
			else if (reaction_ == DownLyingDamageReaction::Back)
			{
				downGettingUpState->DamageReaction(CharacterStateDownGettingUp::DownLyingDamageReaction::Back);
			}
		}

		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateDownLying::Exit()
{
	// ダメージリアクションをリセットする
	reaction_ = DownLyingDamageReaction::None;
}

/// @brief ダメージリアクションを起こす
/// @param hitPosition 
/// @param attacker 
void CharacterStateDownLying::DamageReaction(DownLyingDamageReaction reaction)
{
	// ダウン中着地のSEを再生する
	//soundManager_->SeDownLanding();

	// ダメージリアクションを設定する
	reaction_ = reaction;
	if (reaction_ == DownLyingDamageReaction::Front)
	{
		// 前向きのアニメーションを再生する
		owner_->SetAnimation(hFront_, true, false);
	}
	else if (reaction_ == DownLyingDamageReaction::Back)
	{
		// 後ろ向きのアニメーションを再生する
		owner_->SetAnimation(hBack_, true, false);
	}
}