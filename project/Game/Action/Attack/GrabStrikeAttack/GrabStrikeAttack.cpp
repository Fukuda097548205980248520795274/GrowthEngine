#include "GrabStrikeAttack.h"
#include "Entity/Character/Character.h"

/// @brief コンストラクタ
GrabStrikeAttack::GrabStrikeAttack(Character* character, const GrabStrikeAttackInitData& initData)
	: Attack(character)
{
	// モーションと攻撃時間を設定
	hAttackMotion_ = initData.hAttackAnimation;
	hTargetAnimation_ = initData.hTargetAnimation;
	moveSpeed_ = initData.moveSpeed;
	moveStartTime_ = initData.moveStartTime;
	moveEndTime_ = initData.moveEndTime;
	attackTime_ = initData.attackTime;
	releaseTime_ = initData.releaseTime;
	damageReaction_ = initData.damageReaction;
	isRelease_ = initData.isRelease;
	knockback_ = initData.knockback;
	knockbackDirection_ = initData.knockbackDirection;

	// 攻撃の種類を掴み打撃に設定
	attackType_ = AttackType::GrabStrike;

	// ヒット定義をコピー
	for (const auto& def : initData.hits)
		hits_.push_back(def);
}

/// @brief 実行
void GrabStrikeAttack::Exec()
{
	// 基底クラスの実行
	Attack::Exec();

	// 掴んでいる相手を取得
	grabbedTarget_ = owner_->GetGrabTarget();

	// もし掴んでいる相手がいない場合は、攻撃を終了する
	if (!grabbedTarget_)
		Exit();

	// アニメーションを設定（ループなし）
	owner_->SetAnimation(hAttackMotion_, false, false);
	grabbedTarget_->SetAnimation(hTargetAnimation_, false, false);

	// タイマーとフラグをリセット
	attackTimer_ = 0.0f;
	isReleased_ = false;
}

/// @brief 更新処理
void GrabStrikeAttack::Update()
{
	// もし掴んでいる相手がいない場合は、攻撃を終了する
	if (!grabbedTarget_)
		Exit();

	// タイマーを記録する
	prevTimer_ = attackTimer_;

	// 攻撃タイマーを更新
	attackTimer_ += engine_->GetDeltaTime();

	// ヒットのタイミングになったら、掴んでいる相手にダメージを与える
	for (auto& state : hits_)
	{
		if (attackTimer_ >= state.hitTime && prevTimer_ <= state.hitTime)
		{
			grabbedTarget_->OnGrabDamage(state.damage);
		}
	}

	// 離すタイミングになったら、ダメージリアクションを入れる
	if (isRelease_ && !isReleased_ && attackTimer_ >= releaseTime_)
	{
		if (grabbedTarget_)
		{
			// ターゲットに対してノックバックの方向を計算するためのベクトルを定義する
			Vector3 forward = grabbedTarget_->GetPosition() - owner_->GetPosition();
			forward.y = 0.0f;
			forward = (forward.Length() > 0.0f) ? forward.Normalize() : owner_->GetDirection();

			// ワールドの上方向（Y軸）を定義する
			Vector3 worldUp(0.0f, 1.0f, 0.0f);

			// 前方とワールドの上方向から右方向を計算する
			Vector3 right;
			right.x = worldUp.y * forward.z - worldUp.z * forward.y;
			right.y = worldUp.z * forward.x - worldUp.x * forward.z;
			right.z = worldUp.x * forward.y - worldUp.y * forward.x;
			right = right.Normalize();

			Vector3 up;
			up.x = forward.y * right.z - forward.z * right.y;
			up.y = forward.z * right.x - forward.x * right.z;
			up.z = forward.x * right.y - forward.y * right.x;
			up = up.Normalize();

			// 攻撃の定義に基づいて、ノックバックの方向を計算する
			Vector3 knockBackDirection;
			knockBackDirection.x = right.x * knockbackDirection_.x + up.x * knockbackDirection_.y + forward.x * knockbackDirection_.z;
			knockBackDirection.y = right.y * knockbackDirection_.x + up.y * knockbackDirection_.y + forward.y * knockbackDirection_.z;
			knockBackDirection.z = right.z * knockbackDirection_.x + up.z * knockbackDirection_.y + forward.z * knockbackDirection_.z;
			knockBackDirection = knockBackDirection.Normalize();

			grabbedTarget_->OnDamage(0, damageReaction_, knockback_, knockBackDirection, owner_->GetWorldPosition());

			// Character側の掴み状態を解除する処理を呼ぶ
			owner_->ReleaseGrab();
		}

		isReleased_ = true; // 解除済みフラグを立てる
	}

	// 攻撃時間が経過したら、攻撃を終了する
	if (attackTimer_ >= attackTime_)
	{
		Attack::Update(); // Action::Update()が呼ばれ、Exit処理へ
	}
}

/// @brief リセット
void GrabStrikeAttack::Reset()
{
	Action::Reset();
	attackTimer_ = 0.0f;
	isReleased_ = false;
	grabbedTarget_ = nullptr;
}

/// @brief 終了、中断
void GrabStrikeAttack::Exit()
{
	// もし掴んでいる相手がいる状態で攻撃が終了した場合は、確実に手を離す
	if (isRelease_ && !isReleased_ && grabbedTarget_)
	{
		owner_->ReleaseGrab();
	}

	Action::Exit();
}