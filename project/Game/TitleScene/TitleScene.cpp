#include "TitleScene.h"

/// @brief 初期化
void TitleScene::Initialize()
{
	light_ = std::make_unique<LightDirectional>("TEST");
}

/// @brief 更新処理
void TitleScene::Update()
{
	light_->param_->position = Vector3(0.0f, 10.0f, 0.0f);

	if (engine_->GetKeyTrigger(DIK_SPACE))
	{
		Transition("Game");
	}
}

/// @brief 描画処理
void TitleScene::Draw()
{
	
}