#include "CharacterStateDownStagger.h"
#include "SoundManager/SoundManager.h"
#include "EffectManager/EffectManager.h"
#include "Entity/Character/Character.h"
#include "../CharacterStateDownLying/CharacterStateDownLying.h"

/// @brief コンストラクタ
	/// @param owner 
CharacterStateDownStagger::CharacterStateDownStagger(Character* owner, AnimationHandle hFront, AnimationHandle hBack)
	: CharacterState(owner), hFront_(hFront), hBack_(hBack)
{
}

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateDownStagger::Enter()
{
	// ダメージタイマーをリセットする
	damageTimer_ = maxDamageTime_;
}

/// @brief 更新処理
/// @param dt 
void CharacterStateDownStagger::Update(float dt)
{
	// ダメージタイマーを更新する
	damageTimer_ -= dt;

	// ダメージタイマーが0以下になったら、ダウン状態へ移行する
	if (damageTimer_ <= 0.0f)
	{
		auto stateMachine = owner_->GetStateMachine();
		if (auto nextState = dynamic_cast<CharacterStateDownLying*>(stateMachine->GetState("DownLying")))
		{
			// ダメージリアクションを設定する
			if (reaction_ == DamageReactionType::Front)
			{
				nextState->DamageReaction(CharacterStateDownLying::DamageReactionType::Front);
			}
			else if (reaction_ == DamageReactionType::Back)
			{
				nextState->DamageReaction(CharacterStateDownLying::DamageReactionType::Back);
			}
		}

		// 状態をダウン状態に変更する
		stateMachine->ChangeState("DownLying");

		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateDownStagger::Exit()
{
	// ダメージリアクションをリセットする
	reaction_ = DamageReactionType::None;
}

/// @brief ダメージリアクションを起こす
/// @param hitPosition 
/// @param attacker 
void CharacterStateDownStagger::DamageReaction(DamageReactionType reaction)
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