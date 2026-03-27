#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	sun_ = std::make_unique<LightDirectional>("Sun");

	engine_->LoadCamera3D("MainCamera_TEST");
	engine_->GetCamera3DParam("MainCamera_TEST")->transform.translate = Vector3(0.0f, 0.0f, -20.0f);

	engine_->LoadSprite(engine_->LoadTexture("Assets/Textures/uvChecker.png"), "TEST_Sprite");
	
	engine_->LoadPrimitiveStaticModel(engine_->LoadModel("Assets/Models/AnimatedCube", "AnimatedCube.gltf"), "TEST_Model");

	engine_->LoadPrefabSprite("Test_Sprite_Prefab", 100, engine_->LoadTexture("Assets/Textures/uvChecker.png"));
	a1_ = engine_->CreatePrefab2DInstance<PrefabInstanceSprite>("Test_Sprite_Prefab");
	a2_ = engine_->CreatePrefab2DInstance<PrefabInstanceSprite>("Test_Sprite_Prefab");

	a2_->param_.transform.translate = Vector2(100.0f, 100.0f);
}

/// @brief 更新処理
void GameScene::Update()
{
	if (engine_->GetKeyTrigger(DIK_SPACE))
	{
		Transition("Title");
	}
}

/// @brief 描画処理
void GameScene::Draw()
{
	engine_->DrawDebugCube(Vector3(-5.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	engine_->DrawDebugCube(Vector3(-5.0f, 3.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.5f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f));

	engine_->DrawRender3D("TEST_Model");

	a2_->Draw();
	a1_->Draw();

	//engine_->DrawRender2D("TEST_Sprite");
}