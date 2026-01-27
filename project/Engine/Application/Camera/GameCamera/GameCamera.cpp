#include "GameCamera.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
GameCamera::GameCamera(const std::string& name) : name_(name)
{
	// カメラハンドルを取得する
	hCamera_ = GrowthEngine::GetInstance()->LoadCamera(this, name_);
}

/// @brief カメラを切り替える
void GameCamera::Switch()
{
	// 切り替え
	GrowthEngine::GetInstance()->CameraSwitch(hCamera_);
}