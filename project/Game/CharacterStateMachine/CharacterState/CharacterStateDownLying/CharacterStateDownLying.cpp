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
	// ツリーのリクエストを行う
	TreeRequest();

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
	/// ツリーのリクエストを行う
	HandleBehaviorTreeNotSet();

	// ダメージタイマーを更新する
	damageTimer_ -= dt;

	// ダメージタイマーが0以下になったら、ダウン状態へ移行する
	if (damageTimer_ <= 0.0f)
	{
		auto stateMachine = owner_->GetStateMachine();
		if (auto nextState = static_cast<CharacterStateDownGettingUp*>(stateMachine->GetState("DownGettingUp")))
		{
			// ダメージリアクションの方向を設定する
			if (reaction_ == DamageReactionType::Front)
			{
				nextState->DamageReaction(CharacterStateDownGettingUp::DamageReactionType::Front);
			}
			else if (reaction_ == DamageReactionType::Back)
			{
				nextState->DamageReaction(CharacterStateDownGettingUp::DamageReactionType::Back);
			}
		}

		// 状態をダウン起き上がり状態に変更する
		stateMachine->ChangeState("DownGettingUp");

		// ダメージリアクションをリセットする
		reaction_ = DamageReactionType::None;

		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateDownLying::Exit()
{
	// ダメージリアクションをリセットする
	reaction_ = DamageReactionType::None;
}

/// @brief ダメージリアクションを起こす
/// @param hitPosition 
/// @param attacker 
void CharacterStateDownLying::DamageReaction(DamageReactionType reaction)
{
	// ダウン中着地のSEを再生する
	//soundManager_->SeDownLanding();

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