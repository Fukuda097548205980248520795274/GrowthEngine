#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	engine_->LoadCamera3D("MainCamera");

	engine_->LoadSound("TEST_audio", engine_->LoadAudio("./Assets/Sounds/bgm/season_goes_around.mp3"));
}

/// @brief 更新処理
void GameScene::Update()
{
	
}

/// @brief 描画処理
void GameScene::Draw()
{
	
}