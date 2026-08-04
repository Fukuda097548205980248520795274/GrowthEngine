#include "MainCamera3D.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
MainCamera3D::MainCamera3D(const std::string& name)
	: name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 3Dカメラを読み込む
	handle_ = engine_->LoadCamera3D(name_);

	// 3Dカメラのパラメータを取得する
	param_ = engine_->GetCamera3DParam(handle_);
}

/// @brief 向きを取得する
/// @return 
Vector3 MainCamera3D::GetDirection() const
{
	// 方向ベクトルを計算
	Vector3 forwardDirection = Vector3(0.0f, 0.0f, 1.0f);

	// 回転を適用
	forwardDirection.x = std::cos(param_->transform.rotate.x) * std::sin(param_->transform.rotate.y);
	forwardDirection.z = std::cos(param_->transform.rotate.x) * std::cos(param_->transform.rotate.y);

	// Y軸の回転を適用
	forwardDirection.y = -std::sin(param_->transform.rotate.x);

	return forwardDirection.Normalize();
}

/// @brief 切り替え
void MainCamera3D::Switch()
{
	// 3Dカメラを切り替える
	engine_->Camera3DSwitch(handle_);
}