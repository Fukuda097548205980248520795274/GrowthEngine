#include "TitleScene.h"

/// @brief 初期化
void TitleScene::Initialize()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	engine_->LoadRenderPass("MainPass", [&]()
		{
			
		}
	);
}

/// @brief 更新処理
void TitleScene::Update()
{
	if (engine_->GetKeyTrigger(DIK_SPACE))
	{
		Transition("Game");
	}
}

/// @brief 描画処理
void TitleScene::Draw()
{
	engine_->ExecuteRenderPass("MainPass");
}