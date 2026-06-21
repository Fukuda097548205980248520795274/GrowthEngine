#include "Shake.h"

/// @brief シェイクを開始する
/// @param duration 
void Shake::StartShake(float duration, float magnitude, const Vector3& direction)
{
	// シェイクの経過時間をリセットして、シェイクの総時間を設定する
	duration_ = duration;
	timer_ = duration_;

	// シェイクの強さを設定する
	magnitude_ = magnitude;

	// シェイクの方向を正規化して保存する
	direction_ = direction;
}

/// @brief 更新処理
/// @param dt 
void Shake::Update(float dt)
{
	// シェイクが終了している場合は何もしない
	if (timer_ <= 0.0f)return;

	// 経過時間を減らす
	timer_ -= dt;
}

/// @brief シェイクのオフセットを取得する
/// @return 
Vector3 Shake::GetShakeOffset() const
{
	// シェイクが終了している場合はオフセットなし
	if (timer_ <= 0.0f)return Vector3(0.0f, 0.0f, 0.0f);

	// 経過時間の割合を計算（0から1の範囲）
	float t = timer_ / duration_;

	// シェイクの強さを減衰させる（時間が経つにつれて弱くなる）
	Vector3 offset(0.0f, 0.0f, 0.0f);
	offset.x = GetRandomRange(-magnitude_, magnitude_) * direction_.x * t;
	offset.y = GetRandomRange(-magnitude_, magnitude_) * direction_.y * t;
	offset.z = GetRandomRange(-magnitude_, magnitude_) * direction_.z * t;

	return offset;
}