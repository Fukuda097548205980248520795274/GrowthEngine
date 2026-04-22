#include "ComboAttack.h"
#include "Entity/Character/Player/Player.h"

/// @brief 実行
void ComboAttack::Exec()
{
	// 基底の実行
	Attack::Exec();

	// アニメーションを設定する
	owner_->SetAnimation(hAttackMotion_, true);
}

/// @brief 更新処理
void ComboAttack::Update()
{
	// 現在のアニメーション再生時間を取得する
	float animationTimer = owner_->GetAnimationTimer();

	// コンボキャンセル受付時間内かチェック
	if (animationTimer >= cancelStartTime_ && animationTimer <= cancelEndTime_)
	{
		// 先行入力をチェックするため、所有者がPlayerかどうかを確認
		Player* player = static_cast<Player*>(owner_);
		if (player)
		{
			AttackInputType bufferedInput = player->GetBufferedAttackInput();

			// 弱攻撃の先行入力があり、派生先が設定されている場合
			if (bufferedInput == AttackInputType::Light && nextLightAttack_)
			{
				// 先行入力を消化する
				player->ConsumeBufferedAttackInput();

				// この攻撃を終了して次の攻撃を実行する
				this->Exit();
				nextLightAttack_->Exec();
				return;
			}
			// 強攻撃の先行入力があり、派生先が設定されている場合
			else if (bufferedInput == AttackInputType::Heavy && nextHeavyAttack_)
			{
				// 先行入力を消化する
				player->ConsumeBufferedAttackInput();

				// この攻撃を終了して次の攻撃を実行する
				this->Exit();
				nextHeavyAttack_->Exec();
				return;
			}
		}
	}

	// コンボキャンセル受付時間を過ぎたらこの攻撃を終了する
	if (animationTimer > cancelEndTime_)
	{
		Exit();
	}

	// 既定の更新
	Attack::Update();
}

/// @brief 次の攻撃があるかどうか
/// @return 
bool ComboAttack::HasNextAttack(AttackInputType inputType) const
{
	if (inputType == AttackInputType::Light)
	{
		return nextLightAttack_ != nullptr; // 弱の派生先がセットされていればtrue
	} 
	else if (inputType == AttackInputType::Heavy)
	{
		return nextHeavyAttack_ != nullptr; // 強の派生先がセットされていればtrue
	}
	return false;
}