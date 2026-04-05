#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	engine_->LoadText(engine_->LoadFont("Test", "C:/Windows/Fonts/Arial.ttf", 256), "Text_Test");

	engine_->LoadPrimitiveStaticModel(engine_->LoadModel("./Assets/Models/AnimatedCube" , "AnimatedCube.gltf") , "Test");
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
	engine_->DrawRender3D("Test");

	engine_->DrawRender2D("Text_Test");
}