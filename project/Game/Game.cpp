#include "Game.h"

/// @brief 初期化
void Game::Initialize()
{
	// シーンマネージャの生成
	sceneManager_ = std::make_unique<SceneManager>("Game");
}

/// @brief 更新処理
void Game::Update()
{
	// シーンマネージャの更新
	sceneManager_->Update();
}

/// @brief 描画処理
void Game::Draw()
{
	// シーンマネージャの描画
	sceneManager_->Draw();
}