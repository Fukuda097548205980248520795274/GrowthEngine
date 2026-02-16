#include "GameCamera2D.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
GameCamera2D::GameCamera2D(const std::string& name) : name_(name)
{
	// カメラハンドルを取得する
	hCamera_ = GrowthEngine::GetInstance()->LoadCamera2D(name_);
}

/// @brief カメラを切り替える
void GameCamera2D::Switch()
{
	// 切り替え
	GrowthEngine::GetInstance()->Camera2DSwitch(hCamera_);
}