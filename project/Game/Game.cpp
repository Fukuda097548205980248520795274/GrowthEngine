#include "Game.h"

/// @brief 初期化
void Game::Initialize()
{
	gameCamera_ = std::make_unique<GameCamera3D>("Test");

	light_ = std::make_unique<LightDirectional>("TEST");

	model_ = std::make_unique<PrimitiveSkinningModel>(
		GrowthEngine::GetInstance()->LoadModel("./Assets/Models/walk", "walk.gltf"),
		GrowthEngine::GetInstance()->LoadAnimation("./Assets/Models/walk", "walk.gltf"), 
		GrowthEngine::GetInstance()->LoadSkeleton("./Assets/Models/walk", "walk.gltf"),
		"suzanne");
	model_->param_->modelTransform.translate.x = -5.0f;

	animationModel_ = std::make_unique<PrimitiveAnimationModel>(
		GrowthEngine::GetInstance()->LoadModel("./Assets/Models/AnimatedCube", "AnimatedCube.gltf"),
		GrowthEngine::GetInstance()->LoadAnimation("./Assets/Models/AnimatedCube", "AnimatedCube.gltf"),
		"AnimatedCube");
	animationModel_->param_->modelTransform.translate.x = 5.0f;

	GrowthEngine::GetInstance()->LoadTexture("./Assets/Textures/uvChecker.png");
	
	object_ = std::make_unique<AudioObject>("./Assets/Sounds/bgm/forget_me_not.mp3", 0.4f, true);
	object_->PlayAudio();
}

/// @brief 更新処理
void Game::Update()
{
	light_->param_->position = Vector3(0.0f, 10.0f, 0.0f);

	model_->param_->animation.timer += 1.0f/ 60.0f;
	model_->param_->animation.timer = std::fmod(model_->param_->animation.timer, 1.0f);
}

/// @brief 描画処理
void Game::Draw()
{
	model_->Draw();

	animationModel_->Draw();
}