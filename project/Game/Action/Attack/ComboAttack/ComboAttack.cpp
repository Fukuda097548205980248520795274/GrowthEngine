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

	// 攻撃がヒットしたかどうかをリセットする
	hasHit_ = false;
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

	// 攻撃タイマーが攻撃判定の時間内であれば、攻撃判定を作成・更新する
	if (attackTimer_ >= hitboxStartTime_ && attackTimer_ <= hitboxEndTime_)
	{
		if (!hasHit_)
		{
			// まだ判定が作られていなければ、実体を作成する
			if (hitbox_.collider_ == nullptr)
				hitbox_.collider_ = owner_->GetHitboxGroup()->CreateInstance();

			if (!partName_.empty())
			{
				auto aabb = static_cast<Collision3DInstanceAABB*>(hitbox_.collider_);
				Matrix4x4 boneMatrix = owner_->GetBoneMatrix(partName_);
				aabb->param_->center = Vector3(boneMatrix.m[3][0], boneMatrix.m[3][1], boneMatrix.m[3][2]);
				aabb->param_->radius = Vector3(0.25f, 0.25f, 0.25f); // 仮の半径
			}

			// ヒットしているかチェックする
			if (hitbox_.IsHit())
			{
				// 攻撃時間が終わったら、判定を削除する
				DeleteHitbox();

				// ヒットしたのでフラグを立てる
				hasHit_ = true;
			}
		}
	}
	else
	{
		// 攻撃時間が終わったら、判定を削除する
		DeleteHitbox();
	}


	// 移動時間内であれば移動する
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

/// @brief リセット
void ComboAttack::Reset()
{
	// 基底のリセット
	Action::Reset();

	// 攻撃タイマーを初期化する
	attackTimer_ = 0.0f;

	// 攻撃がヒットしたかどうかをリセットする
	hasHit_ = false;

	// 攻撃判定が残っていれば削除する
	DeleteHitbox();
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

/// @brief 終了、中断
void ComboAttack::Exit()
{
	// 攻撃判定が残っていれば削除する
	DeleteHitbox();

	// 攻撃がヒットしたかどうかをリセットする
	hasHit_ = false;

	// 基底の終了処理
	Attack::Exit();
}

/// @brief 攻撃判定を削除する
void ComboAttack::DeleteHitbox()
{
	if (hitbox_.collider_ != nullptr)
	{
		hitbox_.collider_->Delete();
		hitbox_.collider_ = nullptr;
	}
}