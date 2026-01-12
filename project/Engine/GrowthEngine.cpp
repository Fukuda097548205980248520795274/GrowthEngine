#include "GrowthEngine.h"
#include <cassert>
#include "Log/Log.h"

#pragma comment(lib,"winmm.lib")

// インスタンス
std::unique_ptr<GrowthEngine> GrowthEngine::instance_ = nullptr;

/// @brief インスタンスを取得する
/// @param screenWidth スクリーン横幅
/// @param screenHeight スクリーン縦幅
/// @param title タイトル
/// @return 
GrowthEngine* GrowthEngine::GetInstance(int32_t screenWidth, int32_t screenHeight, const std::string& title)
{
	if (instance_ == nullptr)
	{
		instance_.reset(new GrowthEngine());
		instance_->Initialize(screenWidth, screenHeight, title);
	}

	return instance_.get();
}

/// @brief インスタンスを取得する
/// @return 
GrowthEngine* GrowthEngine::GetInstance()
{
	// インスタンスがないと失敗
	assert(instance_ != nullptr);
	return instance_.get();
}


/// @brief 初期化
/// @param screenWidth 
/// @param screenHeight 
/// @param title 
void GrowthEngine::Initialize(int32_t screenWidth, int32_t screenHeight, const std::string& title)
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