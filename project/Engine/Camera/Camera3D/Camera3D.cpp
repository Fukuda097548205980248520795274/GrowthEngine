#include "Camera3D.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
Engine::Camera3D::Camera3D()
{
	// パラメータの生成
	param_ = std::make_unique<Camera3DData::Param>();

	// トランスフォーム
	param_->transform.rotate = Vector3(0.0f, 0.0f, 0.0f);
	param_->transform.translate = Vector3(0.0f, 0.0f, 0.0f);

	// 設定
	param_->setting.fov = 0.45f;
	param_->setting.nearClip = 0.01f;
	param_->setting.farClip = 800.0f;

	// 画面の幅を取得する
	param_->aspect.width = static_cast<float>(GrowthEngine::GetInstance()->GetScreenWidth());
	param_->aspect.height = static_cast<float>(GrowthEngine::GetInstance()->GetScreenHeight());

	// 正射影行列を作成する
	projectionMatrix_ =
		MakePerspectiveFovMatrix4x4(param_->setting.fov, param_->aspect.width / param_->aspect.height, param_->setting.nearClip, param_->setting.farClip);

	// ビュー正射影行列を作成する
	viewProjectionMatrix_ = worldMatrix_.Inverse() * projectionMatrix_;
}

/// @brief 更新処理
void Engine::Camera3D::Update()
{
	// 3軸の回転を合成する
	quaternion_ =
		ToQuaternion(param_->transform.rotate.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
		ToQuaternion(param_->transform.rotate.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

	// 正規化する
	quaternion_.Normalize();

	// ワールド行列を生成する
	worldMatrix_ = Make3DRotateMatrix4x4(quaternion_) * Make3DTranslateMatrix4x4(param_->transform.translate);

	// 正射影行列を作成する
	projectionMatrix_ =
		MakePerspectiveFovMatrix4x4(param_->setting.fov, param_->aspect.width / param_->aspect.height, param_->setting.nearClip, param_->setting.farClip);

	// ビュー正射影行列を作成する
	viewProjectionMatrix_ = worldMatrix_.Inverse() * projectionMatrix_;
}