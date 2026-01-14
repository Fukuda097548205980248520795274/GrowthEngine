#include "Game.h"

/// @brief コンストラクタ
/// @param screenWidth 
/// @param screenHeight 
/// @param title 
Game::Game(int32_t screenWidth, int32_t screenHeight,const std::string& title)
{
	// エンジンの生成と初期化
	GrowthEngine::GetInstance(screenWidth, screenHeight, title);
}

/// @brief デストラクタ
Game::~Game()
{
	
}

/// @brief 実行
/// @return 
int Game::Run()
{
	// エンジンを取得
	GrowthEngine* engine = GrowthEngine::GetInstance();

	// ゲームループ
	while (engine->GameLoop())
	{
		// 描画前処理
		engine->PreDraw();



		// 描画後処理
		engine->PostDraw();
	}

	return 0;
}