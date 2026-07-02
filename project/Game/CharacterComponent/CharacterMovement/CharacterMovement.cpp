#include "CharacterMovement.h"
#include "Entity/Character/Character.h"

/// @brief 更新処理
/// @param dt 
void CharacterMovement::Update(float dt)
{
	// ノックバックの更新
	if (knockbackVelocity_.Length() > 0.01f)
	{
		// 攻撃中の場合は、ノックバックを無効化する
		if (owner_->IsAttack())
			knockbackVelocity_ = Vector3(0.0f, 0.0f, 0.0f);

		// ノックバックの速度を減衰させる
		knockbackVelocity_ = knockbackVelocity_ * std::pow(0.1f, dt);
	}
	else
	{
		// ノックバックの速度が十分小さくなったら、ノックバックを終了する
		knockbackVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
	}

	WorldTransform3D* worldTransform = owner_->GetWorldTransform();

	// 向きの更新
	if (worldTransform)
	{
		direction_.x = std::sin(worldTransform->rotate_.y);
		direction_.y = 0.0f;
		direction_.z = std::cos(worldTransform->rotate_.y);
	}

	// 速度の更新
	VelocityUpdate(dt);

	// 落下の更新
	FallUpdate(dt);

	// 移動の更新
	if (worldTransform)worldTransform->translate_ += (currentVelocity_ + knockbackVelocity_) * dt;
}

/// @brief XZ平面の移動入力を設定する
/// @param direction 
/// @param maxSpeed 
void CharacterMovement::SetMoveInputXZ(const Vector2& direction, float maxSpeed)
{
	// 入力された方向の長さを計算する
	const float length = direction.Length();

	// 長さが0の場合 や 地面に接していない場合は移動しない
	if (length <= 0.0f || !IsGrounded() || owner_->IsStyleChanging() || owner_->IsAttack())
	{
		targetVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
		return;
	}

	// 長さが1を超える場合は1にクランプする
	const float clampedLength = (length < 1.0f) ? length : 1.0f;
	const float moveSpeed = clampedLength * maxSpeed;

	// 入力された方向を正規化して、目標速度を計算する
	targetVelocity_.x = direction.x * moveSpeed;
	targetVelocity_.y = 0.0f;
	targetVelocity_.z = direction.y * moveSpeed;

	// つかまれていない場合は入力方向を向く。つかんでいる場合は入力方向の逆を向く
	if (!owner_->IsGrabbing())
	{
		// つかまれていない場合は入力方向を向く
		if ((targetVelocity_.x * targetVelocity_.x + targetVelocity_.z * targetVelocity_.z) > kRotateThreshold)
		{
			targetYaw_ = std::atan2(direction.x, direction.y);
			hasTargetYaw_ = true;
		}
	} 
	else
	{
		// つかんでいる場合は入力方向の逆を向く
		if ((targetVelocity_.x * targetVelocity_.x + targetVelocity_.z * targetVelocity_.z) > kRotateThreshold)
		{
			targetYaw_ = std::atan2(-direction.x, -direction.y);
			hasTargetYaw_ = true;
		}
	}
}

/// @brief 移動を停止させる
void CharacterMovement::Stop()
{
	// 目標速度と現在の速度を0にする
	targetVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
	currentVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
}

/// @brief ノックバックを追加する
/// @param velocity 
void CharacterMovement::AddKnockback(const Vector3& velocity)
{
	knockbackVelocity_ = velocity;
}

/// @brief ターゲットの方向を向く
/// @param targetPosition 
void CharacterMovement::TargetDirection(float dt)
{
	Character* target = owner_->GetLockOnTarget();
	WorldTransform3D* worldTransform = owner_->GetWorldTransform();

	// ロックオンしているターゲットの方向を向く処理
	if (target && worldTransform && 
		owner_->IsStance() && !owner_->IsGrabbing() && !owner_->IsIncapacitated() && !owner_->IsAttack())
	{
		// ターゲットの方向を向く
		Vector3 toTarget = target->GetWorldPosition() - worldTransform->GetWorldPosition();
		if (target->IsBlownAway() || target->IsBlownFalling())
			toTarget = target->GetBonePosition(JointType::Root) - worldTransform->GetWorldPosition();

		// Y軸の回転のみを考慮するため、Y成分を0にする
		toTarget.y = 0.0f;

		// ターゲットの方向がある程度ある場合のみ、ターゲットの方向を向くようにする
		if ((toTarget.x * toTarget.x + toTarget.z * toTarget.z) > kRotateThreshold)
		{
			targetYaw_ = std::atan2(toTarget.x, toTarget.z);
			hasTargetYaw_ = true;
		}
	}
	else if(target && worldTransform && owner_->IsPlayer() && owner_->IsStance() && !owner_->IsGrabbing() && !owner_->IsIncapacitated())
	{
		// ターゲットの方向を向く
		Vector3 toTarget = target->GetWorldPosition() - worldTransform->GetWorldPosition();
		if (target->IsBlownAway() || target->IsBlownFalling())
			toTarget = target->GetBonePosition(JointType::Root) - worldTransform->GetWorldPosition();

		// Y軸の回転のみを考慮するため、Y成分を0にする
		toTarget.y = 0.0f;

		// ターゲットの方向がある程度ある場合のみ、ターゲットの方向を向くようにする
		if ((toTarget.x * toTarget.x + toTarget.z * toTarget.z) > kRotateThreshold)
		{
			targetYaw_ = std::atan2(toTarget.x, toTarget.z);
			hasTargetYaw_ = true;
		}
	}


	// ターゲットの方向へ向く処理
	if (hasTargetYaw_)
	{
		float currentYaw = worldTransform->rotate_.y;
		float deltaYaw = std::atan2(std::sin(targetYaw_ - currentYaw), std::cos(targetYaw_ - currentYaw));

		float rotateLerpT = 1.0f - std::exp(-12.0f * dt);

		worldTransform->rotate_.y = currentYaw + deltaYaw * rotateLerpT;

		// 角度が十分近くなったら、ターゲットの方向を向ききったとみなす
		if ((deltaYaw * deltaYaw) <= kRotateThreshold)
		{
			worldTransform->rotate_.y = targetYaw_;
			hasTargetYaw_ = false;
		}
	}
}

/// @brief 落下の更新
/// @param dt 
void CharacterMovement::FallUpdate(float dt)
{
	// ワールドトランスフォームを取得する
	WorldTransform3D* worldTransform = owner_->GetWorldTransform();

	// 重力による落下処理
	if (worldTransform && !isGrounded_)
	{
		// 落下速度を更新する
		velocityY_ += kGravity * dt;
		if (velocityY_ < kMaxFallSpeed) velocityY_ = kMaxFallSpeed;

		// Y方向の位置を更新する
		worldTransform->translate_.y += velocityY_ * dt;
	}
}

/// @brief 着地判定の更新
/// @param dt 
void CharacterMovement::VelocityUpdate(float dt)
{
	// 速度の更新
	const float velocityLerpT = 1.0f - std::exp(-velocityLerpSpeed_ * dt);
	currentVelocity_ = Lerp(currentVelocity_, targetVelocity_, velocityLerpT);
}