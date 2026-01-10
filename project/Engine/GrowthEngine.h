#pragma once

#include <cstdint>
#include <string>
#include <memory>
#include <Windows.h>

namespace Engine
{
	class WinApp;
}

class GrowthEngine
{
public:

	/// @brief インスタンスを取得する
	static GrowthEngine* GetInstance();

	/// @brief インスタンスを取得する
	/// @param screenWidth 
	/// @param screenHeight 
	/// @param title 
	/// @return 
	static GrowthEngine* GetInstance(int32_t screenWidth, int32_t screenHeight, const std::string& title);

	/// @brief ゲームループ
	/// @return 
	bool GameLoop();

	/// @brief 終了処理
	void Finalize();


private:


	/// @brief 初期化
	/// @param screenWidth スクリーンの横幅
	/// @param screenHeight スクリーンの縦幅
	/// @param title タイトル
	void Initialize(int32_t screenWidth, int32_t screenHeight, const std::string& title);


	GrowthEngine() = default;
	~GrowthEngine() = default;
	GrowthEngine(GrowthEngine&) = delete;
	GrowthEngine& operator=(GrowthEngine&) = delete;

	// インスタンス
	static GrowthEngine* instance_;


private:

	// ウィンドウアプリケーション
	Engine::WinApp* winApp_ = nullptr;
};

