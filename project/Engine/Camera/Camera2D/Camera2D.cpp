#include "Camera2D.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
Engine::Camera2D::Camera2D()
{
	// パラメータの生成
	param_ = std::make_unique<Camera2DData::Param>();

	// トランスフォーム
	param_->transform.rotate = 0.0f;
	param_->transform.translate = Vector2(0.0f, 0.0f);

	// 設定
	param_->setting.nearClip = 0.0f;
	param_->setting.farClip = 100.0f;

	// 画面の幅を取得する
	param_->aspect.width = static_cast<float>(GrowthEngine::GetInstance()->GetScreenWidth());
	param_->aspect.height = static_cast<float>(GrowthEngine::GetInstance()->GetScreenHeight());

	// 正射影行列を作成する
	projectionMatrix_ =
		MakeOrthographicMatrix4x4(0.0f, 0.0f, param_->aspect.width, param_->aspect.height, param_->setting.nearClip, param_->setting.farClip);

	// ビュー正射影行列を作成する
	viewProjectionMatrix_ = worldMatrix_.Inverse() * projectionMatrix_;
}

/// @brief 更新処理
void Engine::Camera2D::Update()
{
	// ワールド行列を生成する
	worldMatrix_ = Make3DRotateZMatrix4x4(param_->transform.rotate) * Make3DTranslateMatrix4x4(Vector3(param_->transform.translate.x, param_->transform.translate.y, 0.0f));

	// 正射影行列を作成する
	projectionMatrix_ =
		MakeOrthographicMatrix4x4(0.0f, 0.0f, param_->aspect.width, -param_->aspect.height, param_->setting.nearClip, param_->setting.farClip);

	// ビュー正射影行列を作成する
	viewProjectionMatrix_ = worldMatrix_.Inverse() * projectionMatrix_;
}