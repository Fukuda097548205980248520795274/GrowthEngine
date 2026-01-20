#include "Game.h"

/// @brief 初期化
void Game::Initialize()
{
	GrowthEngine::GetInstance()->LoadTexture("./Resources/Textures/white2x2.png");
	
	object_ = std::make_unique<AudioObject>("./Resources/Sounds/bgm/forget_me_not.mp3", 0.4f, true);
	object_->PlayAudio();
}

/// @brief 更新処理
void Game::Update()
{
	
}

/// @brief 描画処理
void Game::Draw()
{

}