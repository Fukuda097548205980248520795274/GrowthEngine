#pragma once
#include "GrowthEngine.h"

class ContextEngine
{
public:

	/// @brief インスタンスを取得する
	/// @param screenWidth 
	/// @param screenHeight 
	/// @param title 
	/// @return 
	static GrowthEngine* GetEngine(int32_t screenWidth, int32_t screenHeight, const std::string& title);

	/// @brief インスタンスを取得する
	/// @return 
	static GrowthEngine* GetEngine();

	/// @brief 終了処理
	static void Finalize();


private:

	ContextEngine() = default;
	~ContextEngine() = default;
	ContextEngine(ContextEngine&) = delete;
	ContextEngine& operator=(ContextEngine&) = delete;

	// エンジン
	static GrowthEngine* engine_;
};

