#include "WorldTransform3D.h"
#include <cmath>

/// @brief 更新処理
void WorldTransform3D::Update()
{
	// 3軸の回転を合成する
	quaternion_ = 
		ToQuaternion(rotate_.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(rotate_.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(rotate_.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	// 正規化する
	quaternion_.Normalize();

	// ワールド行列を生成する
	worldMatrix_ = Make3DAffineMatrix4x4(scale_, quaternion_, translate_);

	// 親
	if (parent_)
	{
		worldMatrix_ *= parent_->GetWorldMatrix();
	}
}

/// @brief 回転を設定する
/// @param quaternion 
void WorldTransform3D::SetRotate(const Quaternion& quaternion)
{
	// クォータニオンを設定する
	quaternion_ = quaternion;

	// クォータニオンから回転を取得する
	rotate_.x = std::atan2f(2.0f * (quaternion_.w * quaternion_.x + quaternion_.y * quaternion_.z), 1.0f - 2.0f * (quaternion_.x * quaternion_.x + quaternion_.y * quaternion_.y));
	rotate_.y = std::asinf(2.0f * (quaternion_.w * quaternion_.y - quaternion_.z * quaternion_.x));
	rotate_.z = std::atan2f(2.0f * (quaternion_.w * quaternion_.z + quaternion_.x * quaternion_.y), 1.0f - 2.0f * (quaternion_.y * quaternion_.y + quaternion_.z * quaternion_.z));
}

/// @brief ワールド座標を取得する
/// @return 
Vector3 WorldTransform3D::GetWorldPosition()const
{
	return Vector3(worldMatrix_.m[3][0], worldMatrix_.m[3][1], worldMatrix_.m[3][2]);
}