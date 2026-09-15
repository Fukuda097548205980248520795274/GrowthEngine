#include "CharacterStateNone.h"
#include "Entity/Character/Character.h"

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateNone::Enter()
{
	// ツリーのリクエストを行う
	TreeRequest();
}

/// @brief 更新処理
/// @param dt 
void CharacterStateNone::Update(float dt)
{
    /// ツリーのリクエストを行う
    HandleBehaviorTreeNotSet();


	// スタイルチェンジ中でない場合は、通常のモーションを再生する
	if (!owner_->IsStyleChanging())
	{
		if (!owner_->GetCurrentAttack() && !owner_->IsDamageReaction() && !owner_->IsGrabbed() && !owner_->IsGuard() && !owner_->IsRepelling() && !owner_->IsDeflecting() &&
			!owner_->IsAvoid() && !owner_->IsDown() && !owner_->IsDash() && !owner_->IsTelegraph())
		{
			// 立ちモーションを再生する
			owner_->SetAnimation(hStandMotion_, false, true);

			//　移動している場合は歩きモーションを再生する
			if (owner_->GetMovement()->GetTargetVelocity().Length() > 0.0f)
				owner_->SetAnimation(hWalkMotion_, false, true);

			// 構え中は構えモーションを優先して再生する
			if (owner_->IsStance())
				owner_->SetAnimation(hStanceMotion_, false, true);
		}
	}
}