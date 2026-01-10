#include "ContextEngine.h"
#include <cassert>

// エンジン
GrowthEngine* ContextEngine::engine_ = nullptr;

/// @brief インスタンスを取得する
/// @param screenWidth 
/// @param screenHeight 
/// @param title 
/// @return 
GrowthEngine* ContextEngine::GetEngine(int32_t screenWidth, int32_t screenHeight, const std::string& title)
{
	if (engine_ == nullptr)
	{
		engine_ = new GrowthEngine(screenWidth, screenHeight, title);
	}
	return engine_;
}

/// @brief インスタンスを取得する
/// @return 
GrowthEngine* ContextEngine::GetEngine()
{
	assert(engine_ != nullptr);
	return engine_;
}

/// @brief 終了処理
void ContextEngine::Finalize()
{
	delete engine_;
	engine_ = nullptr;
}