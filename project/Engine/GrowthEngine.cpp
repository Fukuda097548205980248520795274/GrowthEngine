#include "GrowthEngine.h"
#include <cassert>

#pragma comment(lib,"winmm.lib")


/// @brief コンストラクタ
/// @param screenWidth 
/// @param screenHeight 
/// @param title 
GrowthEngine::GrowthEngine(int32_t screenWidth, int32_t screenHeight, const std::string& title)
{
	// ウィンドウアプリケーションの生成と初期化
	winApp_ = std::make_unique<Engine::WinApp>();
	winApp_->Initialize(screenWidth, screenHeight, title);
}

/// @brief デストラクタ
GrowthEngine::~GrowthEngine()
{
	// ウィンドウアプリケーション
	winApp_.reset();
	winApp_ = nullptr;
}