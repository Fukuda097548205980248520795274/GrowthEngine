#include "SpinThrowAttack.h"
#include "Entity/Character/Character.h"

SpinThrowAttack::SpinThrowAttack(Character* character, const SpinThrowAttackInitData& initData)
	: Attack(character)
{
	// 初期化データをメンバ変数にコピーする
	hAttackMotion_ = initData.hAttackMotion;
	attackTime_ = initData.attackTime;
	moveSpeed_ = initData.moveSpeed;
	moveStartTime_ = initData.moveStartTime;
	moveEndTime_ = initData.moveEndTime;
	jointType_ = initData.jointType;
	damage_ = initData.damage;
	damageReaction_ = initData.damageReaction;
	knockback_ = initData.knockback;
	hitboxStartTime_ = initData.hitboxStartTime;
	hitboxEndTime_ = initData.hitboxEndTime;
	knockbackDirection_ = initData.knockbackDirection.Normalize();

	attackType_ = AttackType::SpinThrow;
}

void SpinThrowAttack::Exec()
{
	Attack::Exec();

	// 技開始アニメーションを設定
	owner_->SetAnimation(hAttackMotion_, true, false);

	// 初期化
	attackTimer_ = 0.0f;
	isSpinning_ = false;
	hasThrown_ = false;
	grabbedTarget_ = nullptr;

	// 初期方向は、技を出した瞬間のキャラクターの向きに設定しておく
	throwDirection_ = owner_->GetDirection();
}

void SpinThrowAttack::Update()
{
	// タイマーの進行
	attackTimer_ += engine_->GetDeltaTime();

	// ------------------------------------------
	// 1. 掴み判定フェーズ（まだ回転状態ではない場合）
	// ------------------------------------------
	if (!isSpinning_ && attackTimer_ >= hitboxStartTime_ && attackTimer_ <= hitboxEndTime_)
	{
		// ※GrabAttackと同じようにHitboxやColliderを使って判定を行う想定です
		const auto& characters = Character::GetCharacters();
		for (auto* target : characters)
		{
			if (target == owner_ || target->GetCharacterTag() == owner_->GetCharacterTag() || target->IsDead()) continue;

			// 仮の判定（実際のAppColliderのヒット判定に置き換えてください）
			float distSq = (target->GetPosition() - owner_->GetPosition()).Length();
			if (distSq < 1.5f * 1.5f) // 1.5m以内の敵を掴む
			{
				isSpinning_ = true;
				grabbedTarget_ = target;

				// 相手を「掴まれ状態」にする処理を呼ぶ
				// owner_->ExecuteGrab(target, ...); など
				break;
			}
		}
	}

	// ------------------------------------------
	// 2. 回転・自動移動フェーズ
	// ------------------------------------------
	if (isSpinning_ && !hasThrown_)
	{
		// キャラクターの向き(モデルの正面)を移動方向に合わせる
		owner_->SetDirection(throwDirection_);

		// 自動移動（指定された時間内のみ）
		if (attackTimer_ >= moveStartTime_ && attackTimer_ <= moveEndTime_)
		{
			Vector3 position = owner_->GetPosition();
			position += moveSpeed_ * (throwDirection_ * engine_->GetDeltaTime());
			owner_->SetPosition(position);
		}

		// ※ ここで周囲への「巻き込み当たり判定」を発生させ続けると、ジャイアントスイングのような技になります
		// 例: AppColliderの球体判定を自身の位置に置き、Hitした敵(grabbedTarget_以外)にダメージを与える

		// 掴んでいる敵の座標を、自身の手に追従させる（もしくはCharacter側のUpdateで処理してもOK）
		if (grabbedTarget_)
		{
			// Vector3 handPos = owner_->GetJointPosition(jointType_);
			// grabbedTarget_->SetPosition(handPos);
		}
	}

	// ------------------------------------------
	// 3. 投擲フェーズ（技の終盤）
	// ------------------------------------------
	if (attackTimer_ >= attackTime_)
	{
		if (isSpinning_ && !hasThrown_)
		{
			if (grabbedTarget_)
			{
				// ターゲットを投げる方向（throwDirection_）にノックバック＆ダメージ
				// grabbedTarget_->ApplyDamage(damage_, damageReaction_);
				// grabbedTarget_->ApplyKnockback(throwDirection_, knockback_);
			}
			hasThrown_ = true;
		}

		// 基底クラスのUpdateを呼んで isSuccess_ を true にし、Action::Exit() を実行する
		Attack::Update();
	}
}

void SpinThrowAttack::Reset()
{
	Action::Reset();
	attackTimer_ = 0.0f;
	isSpinning_ = false;
	hasThrown_ = false;
	grabbedTarget_ = nullptr;
}

void SpinThrowAttack::Exit()
{
	// 攻撃が割り込まれた（怯んだ等）場合のセーフティ
	if (grabbedTarget_ && !hasThrown_)
	{
		// 掴んでいる敵を解放する（地面に落とす等）
		// grabbedTarget_->ReleaseGrab();
		grabbedTarget_ = nullptr;
	}

	Attack::Exit();
}