#include "Game.h"

/// @brief 初期化
void Game::Initialize()
{
	gameCamera2D_ = std::make_unique<GameCamera2D>("TEST");

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

	// プレハブスプライト
	sprite_ = std::make_unique<PrefabBaseSprite>(GrowthEngine::GetInstance()->LoadTexture("./Assets/Textures/uvChecker.png"), 100, "Sprite");
	instanceSprite1_ = sprite_->CreateInstance();
	instanceSprite2_ = sprite_->CreateInstance();
	instanceSprite2_->param_.transform.translate.x = 100.0f;
	
	
	object_ = std::make_unique<AudioObject>("./Assets/Sounds/bgm/forget_me_not.mp3", 0.4f, true);
	object_->PlayAudio();

	radialBlur_ = std::make_unique<PostEffectRadialBlur>("Test");
}

/// @brief 更新処理
void Game::Update()
{
	sprite_->Update();

	light_->param_->position = Vector3(0.0f, 10.0f, 0.0f);

	model_->param_->animation.timer += 1.0f/ 60.0f;
	model_->param_->animation.timer = std::fmod(model_->param_->animation.timer, 1.0f);
}

/// @brief 描画処理
void Game::Draw()
{
	model_->Draw();

	animationModel_->Draw();

	instanceSprite1_->Draw();
	instanceSprite2_->Draw();

	sprite_->Draw();

	radialBlur_->Draw();
}