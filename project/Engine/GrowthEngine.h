#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <Windows.h>

#include "WinApp/WinApp.h"

class GrowthEngine
{
public:

	/// @brief コンストラクタ
	/// @param screenWidth 
	/// @param screenHeight 
	/// @param title 
	GrowthEngine(int32_t screenWidth, int32_t screenHeight, const std::string& title);

	/// @brief デストラクタ
	~GrowthEngine();

	/// @brief ゲームループ
	/// @return 
	bool GameLoop() { return winApp_->ProcessMessage(); }


private:

	// ウィンドウアプリケーション
	std::unique_ptr<Engine::WinApp> winApp_ = nullptr;
};

