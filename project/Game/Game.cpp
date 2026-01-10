#include "Game.h"

/// @brief コンストラクタ
/// @param screenWidth 
/// @param screenHeight 
/// @param title 
Game::Game(int32_t screenWidth, int32_t screenHeight,const std::string& title)
{
	// アプリケーション用エンジンの生成と初期化
	ContextEngine::GetEngine(screenWidth, screenHeight, title);
}

/// @brief デストラクタ
Game::~Game()
{
	// アプリケーション用エンジンの終了処理
	ContextEngine::Finalize();
}

/// @brief 実行
/// @return 
int Game::Run()
{
	// エンジンを取得
	GrowthEngine* engine = ContextEngine::GetEngine();

	// ゲームループ
	while (engine->GameLoop())
	{

	}

	return 0;
}