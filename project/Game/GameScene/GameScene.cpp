#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	light_ = std::make_unique<LightDirectional>("TEST");


	gameCamera2D_ = std::make_unique<GameCamera2D>("TEST");

	gameCamera_ = std::make_unique<GameCamera3D>("Test");

	model_ = std::make_unique<PrimitiveSkinningModel>(
		GrowthEngine::GetInstance()->LoadModel("./Assets/Models/walk", "walk.gltf"),
		GrowthEngine::GetInstance()->LoadAnimation("./Assets/Models/walk", "walk.gltf"),
		GrowthEngine::GetInstance()->LoadSkeleton("./Assets/Models/walk", "walk.gltf"),
		"suzanne");

	animationModel_ = std::make_unique<PrimitiveAnimationModel>(
		GrowthEngine::GetInstance()->LoadModel("./Assets/Models/AnimatedCube", "AnimatedCube.gltf"),
		GrowthEngine::GetInstance()->LoadAnimation("./Assets/Models/AnimatedCube", "AnimatedCube.gltf"),
		"AnimatedCube");
	animationModel_->param_->modelTransform.translate.y = -1.0f;


	modelPrefab_ = std::make_unique<PrefabBaseStaticModel>(GrowthEngine::GetInstance()->LoadModel("./Assets/Models/plane", "plame.obj"), 100, "TEST");
	instance1_ = modelPrefab_->CreateInstance();
	instance2_ = modelPrefab_->CreateInstance();
	instance1_->param_.modelTransform.translate.y = -1.0f;
	instance2_->param_.modelTransform.translate.y = -2.0f;


	object_ = std::make_unique<AudioObject>("./Assets/Sounds/bgm/forget_me_not.mp3", 0.4f, true);
	object_->PlayAudio();
}

/// @brief 更新処理
void GameScene::Update()
{
	light_->param_->position = Vector3(0.0f, 10.0f, 0.0f);

	model_->param_->animation.timer += 1.0f / 60.0f;
	model_->param_->animation.timer = std::fmod(model_->param_->animation.timer, 1.0f);

	if (engine_->GetKeyTrigger(DIK_SPACE))
	{
		Transition("Title");
	}
}

/// @brief 描画処理
void GameScene::Draw()
{
	model_->Draw();

	animationModel_->Draw();

	instance1_->Draw();
	instance2_->Draw();
	modelPrefab_->Draw();
}