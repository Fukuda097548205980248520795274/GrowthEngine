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

/// @brief 切り替え
void MainCamera3D::Switch()
{
	// 3Dカメラを切り替える
	engine_->Camera3DSwitch(handle_);
}