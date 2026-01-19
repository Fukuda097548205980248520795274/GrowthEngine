#include "WorldTransform3D.h"

/// @brief 更新処理
void WorldTransform3D::Update()
{
	// 3軸の回転を合成する
	quaternion_ = 
		ToQuaternion(rotate_.x, Vector3(1.0f, 0.0, 0.0f)).Normalize() * 
		ToQuaternion(rotate_.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() * 
		ToQuaternion(rotate_.z, Vector3(0.0f, 0.0, 1.0f)).Normalize();

	// 正規化する
	quaternion_.Normalize();

	// ワールド行列を生成する
	worldMatrix_ = MakeAffineMatrix4x4(scale_, quaternion_, translate_);

	// 親
	if (parent_)
	{
		worldMatrix_ *= parent_->GetWorldMatrix();
	}
}

/// @brief ワールド座標を取得する
/// @return 
Vector3 WorldTransform3D::GetWorldPosition()const
{
	return Vector3(worldMatrix_.m[3][0], worldMatrix_.m[3][1], worldMatrix_.m[3][2]);
}