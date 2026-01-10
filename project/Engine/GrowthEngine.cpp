#include "GrowthEngine.h"

#include <cassert>
#include "WinApp/WinApp.h"

#pragma comment(lib,"winmm.lib")


// インスタンスの初期化
GrowthEngine* GrowthEngine::instance_ = nullptr;

/// @brief インスタンスを取得する
GrowthEngine* GrowthEngine::GetInstance()
{
	// インスタンスがないと取得できない
	assert(instance_ != nullptr);
	return instance_;
}

/// @brief インスタンスを取得する
/// @param screenWidth 
/// @param screenHeight 
/// @param title 
/// @return 
GrowthEngine* GrowthEngine::GetInstance(int32_t screenWidth, int32_t screenHeight, const std::string& title)
{
	// インスタンスがないとき
	if (instance_ == nullptr)
	{
		instance_ = new GrowthEngine();
		instance_->Initialize(screenWidth, screenHeight, title);
	}

	return instance_;
}


/// @brief 初期化
/// @param screenWidth スクリーンの横幅
/// @param screenHeight スクリーンの縦幅
/// @param title タイトル
void GrowthEngine::Initialize(int32_t screenWidth, int32_t screenHeight, const std::string& title)
{
	// ウィンドウアプリケーションの生成と初期化
	winApp_ = new Engine::WinApp();
	winApp_->Initialize(screenWidth, screenHeight, title);
}

/// @brief ゲームループ
/// @return 
bool GrowthEngine::GameLoop() { return winApp_->ProcessMessage(); }

/// @brief 終了処理
void GrowthEngine::Finalize()
{
	// ウィンドウアプリケーション
	delete winApp_;
	winApp_ = nullptr;

	// エンジンのインスタンス
	delete instance_;
	instance_ = nullptr;
}