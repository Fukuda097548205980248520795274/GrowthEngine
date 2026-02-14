#include "Game.h"

/// @brief 初期化
void Game::Initialize()
{
	gameCamera_ = std::make_unique<GameCamera>("Test");

	light_ = std::make_unique<LightDirectional>("TEST");

	model_ = std::make_unique<PrimitiveStaticModel>(GrowthEngine::GetInstance()->LoadModel("./Assets/Models/suzanne", "suzanne.gltf"), "Suzanne");
	plane_ = std::make_unique<PrimitiveStaticModel>(GrowthEngine::GetInstance()->LoadModel("./Assets/Models/plane", "plame.obj"), "Plane");

	GrowthEngine::GetInstance()->LoadTexture("./Assets/Textures/uvChecker.png");
	
	object_ = std::make_unique<AudioObject>("./Assets/Sounds/bgm/forget_me_not.mp3", 0.4f, true);
	object_->PlayAudio();
}

/// @brief 更新処理
void Game::Update()
{
	light_->param_->position = Vector3(0.0f, 10.0f, 0.0f);

	model_->param_->modelTransform.rotate.y += 0.01f;
}

/// @brief 描画処理
void Game::Draw()
{

	model_->Draw();
	plane_->Draw();
}