#include "Game.h"

/// @brief 初期化
void Game::Initialize()
{
	gameCamera_ = std::make_unique<GameCamera>("Test");

	

	model_ = std::make_unique<PrimitiveStaticModel>(GrowthEngine::GetInstance()->LoadModel("./Assets/Models/suzanne", "suzanne.obj"), "Suzanne");
	GrowthEngine::GetInstance()->LoadTexture("./Assets/Textures/uvChecker.png");
	
	object_ = std::make_unique<AudioObject>("./Assets/Sounds/bgm/forget_me_not.mp3", 0.4f, true);
	object_->PlayAudio();
}

/// @brief 更新処理
void Game::Update()
{
	gameCamera_->transform_.translate->z = -20.0f;
}

/// @brief 描画処理
void Game::Draw()
{
	model_->Draw();
}