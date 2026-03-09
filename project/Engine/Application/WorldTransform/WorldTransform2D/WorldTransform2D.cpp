#include "WorldTransform2D.h"

/// @brief 更新処理
void WorldTransform2D::Update()
{
	// 3軸の回転を合成する
	quaternion_ = ToQuaternion(rotate_, Vector3(0.0f, 0.0, 1.0f)).Normalize();

	// 正規化する
	quaternion_.Normalize();

	// ワールド行列を生成する
	worldMatrix_ = Make2DScaleMatrix4x4(scale_) * Make3DRotateMatrix4x4(quaternion_) * Make2DTranslateMatrix4x4(translate_);

	// 親
	if (parent_)
	{
		worldMatrix_ *= parent_->GetWorldMatrix();
	}
}

/// @brief ワールド座標を取得する
/// @return 
Vector2 WorldTransform2D::GetWorldPosition()const
{
	return Vector2(worldMatrix_.m[3][0], worldMatrix_.m[3][1]);
}