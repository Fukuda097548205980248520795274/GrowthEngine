#include "ComboAttack.h"
#include "Entity/Character/Player/Player.h"

/// @brief 実行
void ComboAttack::Exec()
{
	// 基底の実行
	Attack::Exec();

	// アニメーションを設定する
	owner_->SetAnimation(hAttackMotion_, true , false);

	// 攻撃タイマーを初期化する
	attackTimer_ = 0.0f;
}

/// @brief 更新処理
void ComboAttack::Update()
{
	// コンボキャンセル受付時間内かチェック
	if (attackTimer_ >= cancelStartTime_ && attackTimer_ <= cancelEndTime_)
	{
		if (owner_)
		{
			AttackInputType bufferedInput = owner_->GetBufferedAttackInput();

			// 弱攻撃の先行入力があり、派生先が設定されている場合
			if (bufferedInput == AttackInputType::Light && nextLightAttack_)
			{
				// 先行入力を消化する
				owner_->ConsumeBufferedAttackInput();

				// この攻撃を終了して次の攻撃を実行する
				this->Exit();
				nextLightAttack_->Exec();
				return;
			}
			// 強攻撃の先行入力があり、派生先が設定されている場合
			else if (bufferedInput == AttackInputType::Heavy && nextHeavyAttack_)
			{
				// 先行入力を消化する
				owner_->ConsumeBufferedAttackInput();

				// この攻撃を終了して次の攻撃を実行する
				this->Exit();
				nextHeavyAttack_->Exec();
				return;
			}
		}
	}

	// 攻撃タイマーを更新する
	attackTimer_ += engine_->GetDeltaTime();

	if(attackTimer_ >= moveStartTime_ && attackTimer_ <= moveEndTime_)
	{
		// 方向と位置を取得する
		Vector3 direction = owner_->GetDirection();
		Vector3 position = owner_->GetPosition();

		// 位置を更新する
		position += moveSpeed_ * (direction * engine_->GetDeltaTime());
		owner_->SetPosition(position);
	}

	// コンボキャンセル受付時間を過ぎたらこの攻撃を終了する
	if (attackTimer_ >= attackTime_)
	{
		// 既定の更新
		Attack::Update();
	}
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