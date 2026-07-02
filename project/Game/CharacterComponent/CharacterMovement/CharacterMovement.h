#pragma once
#include "GrowthEngine.h"

class Character;

class CharacterMovement
{
public:

	/// @brief コンストラクタ
	CharacterMovement(Character* owner) : owner_(owner) {}

	/// @brief デストラクタ
	~CharacterMovement() = default;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt);

	/// @brief XZ平面の移動入力を設定する
	/// @param direction 
	/// @param maxSpeed 
	void SetMoveInputXZ(const Vector2& direction, float maxSpeed);

	/// @brief 移動を停止させる
	void Stop();

	/// @brief ノックバックを追加する
	/// @param velocity 
	void AddKnockback(const Vector3& velocity);

	/// @brief ノックバックの速度を取得する
	/// @return 
	Vector3 GetKnockbackVelocity() const { return knockbackVelocity_; }

	/// @brief Y方向の速度を取得する
	/// @return 
	float GetVelocityY() const { return velocityY_; }

	/// @brief ターゲットの方向を向く
	/// @param targetPosition 
	void TargetDirection(float dt);

	/// @brief 現在向いている方向ベクトルを取得する
	/// @return 
	Vector3 GetDirection() const { return direction_; }

	/// @brief 向きを設定する
	/// @param direction 
	void SetDirection(const Vector3& direction) { direction_ = direction.Normalize(); }

	/// @brief 目標速度を取得する
	/// @return 
	Vector3 GetTargetVelocity() const { return targetVelocity_; }

	/// @brief 現在の速度を取得する
	/// @return 
	Vector3 GetCurrentVelocity() const { return currentVelocity_; }

	/// @brief 着地状態を取得する
	/// @return 
	bool IsGrounded() const { return isGrounded_; }

	/// @brief 着地状態を設定する
	/// @param isGrounded 
	void SetGrounded(bool isGrounded) { isGrounded_ = isGrounded; }

	/// @brief Y方向の速度を取得する
	/// @param velocityY 
	void SetVelocityY(float velocityY) { velocityY_ = velocityY; }

	/// @brief 目標Y軸回転角度を設定する
	/// @param rotationY 
	void SetTargetRotationY(float rotationY) { targetRotationY_ = rotationY; }

	/// @brief 目標Y軸回転角度を取得する
	/// @return 
	float GetTargetRotationY() const { return targetRotationY_; }

	/// @brief 回転の補間スピードを取得する
	/// @return 
	float GetRotationSpeed() const { return rotationSpeed_; }

private:

	/// @brief 落下の更新
	/// @param deltaTime 
	void FallUpdate(float dt);

	/// @brief 着地判定の更新
	/// @param deltaTime 
	void VelocityUpdate(float dt);


private:

	/// @brief 所有者のインスタンス
	Character* owner_ = nullptr;


private:

	// @brief 現在の移動方向（正規化済み）
	Vector3 direction_ = Vector3(0.0f, 0.0f, 1.0f);

	// @brief 目標速度
	Vector3 targetVelocity_ = Vector3(0.0f, 0.0f, 0.0f);

	// @brief 現在の速度
	Vector3 currentVelocity_ = Vector3(0.0f, 0.0f, 0.0f);

	// @brief 速度補間の応答速度(1秒あたり)
	float velocityLerpSpeed_ = 6.0f;


private:

	// @brief 現在の目標Y軸回転角度（ラジアン）
	float targetRotationY_ = 0.0f;

	// @brief 回転の補間スピード（数値が大きいほど素早く振り向く）
	float rotationSpeed_ = 10.0f;

	// @brief 目標Y回転
	float targetYaw_ = 0.0f;

	// @brief 目標回転が有効かどうか
	bool hasTargetYaw_ = false;

	// @brief 回転の閾値（この値以下の差分は無視する）
	static constexpr float kRotateThreshold = 0.0001f;


private:

	// ノックバックの速度
	Vector3 knockbackVelocity_ = Vector3(0.0f, 0.0f, 0.0f);


private:

	// Y方向の速度
	float velocityY_ = 0.0f;

	// 地面に接地しているかどうか
	bool isGrounded_ = false;

	// 重力加速度（m/s^2）
	const float kGravity = -9.8f * 0.75f;

	// 最大落下速度
	const float kMaxFallSpeed = -20.0f;
};