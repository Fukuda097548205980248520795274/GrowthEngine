#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	
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
	
}