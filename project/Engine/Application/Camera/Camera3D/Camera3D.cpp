#include "Camera3D.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
Camera3D::Camera3D()
{
	// 画面の幅を取得する
	width_ = static_cast<float>(GrowthEngine::GetInstance()->GetScreenWidth());
	height_ = static_cast<float>(GrowthEngine::GetInstance()->GetScreenHeight());

	// 正射影行列を作成する
	projectionMatrix_ = MakePerspectiveFovMatrix4x4(fov_, width_ / height_, nearClip_, farClip_);

	// ビュー正射影行列を作成する
	viewProjectionMatrix_ = worldMatrix_.Inverse() * projectionMatrix_;
}

/// @brief 更新処理
void Camera3D::Update()
{
	// 3軸の回転を合成する
	quaternion_ =
		ToQuaternion(rotate_.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(rotate_.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(rotate_.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	// 正規化する
	quaternion_.Normalize();

	// ワールド行列を生成する
	worldMatrix_ = Make3DRotateMatrix4x4(quaternion_) * Make3DTranslateMatrix4x4(translate_);

	// 正射影行列を作成する
	projectionMatrix_ = MakePerspectiveFovMatrix4x4(fov_, width_ / height_, nearClip_, farClip_);

	// ビュー正射影行列を作成する
	viewProjectionMatrix_ = worldMatrix_.Inverse() * projectionMatrix_;
}

/// @brief ワールド座標を取得する
/// @return 
Vector3 Camera3D::GetWorldPosition()const
{
	return Vector3(worldMatrix_.m[3][0], worldMatrix_.m[3][1], worldMatrix_.m[3][2]);
}