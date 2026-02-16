#include "GameCamera3D.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
GameCamera3D::GameCamera3D(const std::string& name) : name_(name)
{
	// カメラハンドルを取得する
	hCamera_ = GrowthEngine::GetInstance()->LoadCamera(name_);
}

/// @brief カメラを切り替える
void GameCamera3D::Switch()
{
	// 切り替え
	GrowthEngine::GetInstance()->CameraSwitch(hCamera_);
}