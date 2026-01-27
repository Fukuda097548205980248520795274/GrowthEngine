#include "Game.h"

/// @brief 初期化
void Game::Initialize()
{
	gameCamera_ = std::make_unique<GameCamera>("Test");

	GrowthEngine::GetInstance()->LoadTexture("./Resources/Textures/white2x2.png");
	GrowthEngine::GetInstance()->LoadTexture("./Resources/Textures/uvChecker.png");
	GrowthEngine::GetInstance()->LoadAnimation("./Resources/Models/AnimatedCube", "AnimatedCube.gltf");

	model_ = std::make_unique<PrimitiveStaticModel>(GrowthEngine::GetInstance()->LoadModel("./Resources/Models/suzanne", "suzanne.obj"), "Suzanne");
	
	object_ = std::make_unique<AudioObject>("./Resources/Sounds/bgm/forget_me_not.mp3", 0.4f, true);
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