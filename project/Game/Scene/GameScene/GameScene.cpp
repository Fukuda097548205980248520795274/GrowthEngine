#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	sun_ = std::make_unique<LightDirectional>("Sun");

	engine_->LoadCamera3D("MainCamera_TEST");
	engine_->GetCamera3DParam("MainCamera_TEST")->transform.translate = Vector3(0.0f, 0.0f, -20.0f);

	engine_->LoadSprite(engine_->LoadTexture("Assets/Textures/uvChecker.png"), "TEST_Sprite");
	
	engine_->LoadPrimitiveStaticModel(engine_->LoadModel("Assets/Models/AnimatedCube", "AnimatedCube.gltf"), "TEST_Model");
}

/// @brief 更新処理
void GameScene::Update()
{
	
}

/// @brief 描画処理
void GameScene::Draw()
{
	engine_->DrawDebugCube(Vector3(-5.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	engine_->DrawDebugCube(Vector3(-5.0f, 3.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.5f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f));

	engine_->DrawRender3D("TEST_Model");

	engine_->DrawRender2D("TEST_Sprite");
}