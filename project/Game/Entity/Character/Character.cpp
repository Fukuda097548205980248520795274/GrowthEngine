#include "Character.h"


#include <algorithm>
#include <cmath>

/// @brief 
/// @param position 
Character::Character(const InitData& initData)
	: Entity()
{
	// タグを指定する
	entityTag_ = EntityTag::Character;

	// 位置
	worldTransform_->translate_ = initData.position;

	// 体力
	hp_ = initData.hp;

	// モデル
	if(initData.model_)
		model_ = initData.model_;
}

/// @brief 更新処理
void Character::Update()
{
	// 目標回転が有効な場合、現在の回転と目標回転の差を計算して、線形補間で回転を更新する
	if (hasTargetYaw_)
	{
		// 現在の回転と目標回転の差を計算する
		const float currentYaw = worldTransform_->rotate_.y;
		const float deltaYaw = std::atan2(std::sin(targetYaw_ - currentYaw), std::cos(targetYaw_ - currentYaw));

		// 目標回転に向かって線形補間で回転を更新する
		constexpr float kRotateLerpSpeed = 0.2f;
		worldTransform_->rotate_.y = currentYaw + deltaYaw * kRotateLerpSpeed;

		// 回転が目標回転に十分近い場合、回転を目標回転に設定して、目標回転を無効にする
		if ((deltaYaw * deltaYaw) <= kRotateThreshold)
		{
			worldTransform_->rotate_.y = targetYaw_;
			hasTargetYaw_ = false;
		}
	}

	// 速度の更新
	currentVelocity_ = Lerp(currentVelocity_, targetVelocity_, velocityLerpSpeed_);

	// 位置の更新
	worldTransform_->translate_ += currentVelocity_;

	// 基底クラスの更新
	Entity::Update();
}

/// @brief 移動を停止させる
void Character::MoveStop()
{
	targetVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
	currentVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
}

/// @brief XZ平面の移動入力を設定する
/// @param direction
/// @param maxSpeed
void Character::SetMoveInputXZ(const Vector2& direction, float maxSpeed)
{
	// 入力された方向の長さを計算する
	const float length = direction.Length();

	// 長さが0以下の場合、目標速度を0にして終了する
	if (length <= 0.0f)
	{
		targetVelocity_ = Vector3(0.0f, 0.0f, 0.0f);
		return;
	}

	const float clampedLength = (length < 1.0f) ? length : 1.0f;
	const float moveSpeed = clampedLength * maxSpeed;

	targetVelocity_.x = direction.x * moveSpeed;
	targetVelocity_.y = 0.0f;
	targetVelocity_.z = direction.y * moveSpeed;

	if ((targetVelocity_.x * targetVelocity_.x + targetVelocity_.z * targetVelocity_.z) > kRotateThreshold)
	{
		targetYaw_ = std::atan2(direction.x, direction.y);
		hasTargetYaw_ = true;
	}
}