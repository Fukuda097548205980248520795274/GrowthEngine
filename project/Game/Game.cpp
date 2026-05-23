#include "Game.h"
#include "MotionManager/MotionManager.h"
#include "SoundManager/SoundManager.h"

/// @brief 初期化
void Game::Initialize()
{
	// シーンマネージャの生成
	sceneManager_ = std::make_unique<SceneManager>("Title");

	// マネージャの生成と初期化
	MotionManager* motionManager = MotionManager::GetInstance();
	SoundManager* soundManager = SoundManager::GetInstance();

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