#include "TitleScene.h"

/// @brief 初期化
void TitleScene::Initialize()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	engine_->LoadPostEffect("Test", Engine::PostEffect::Type::Dissolve);
}

/// @brief 更新処理
void TitleScene::Update()
{

}

/// @brief 描画処理
void TitleScene::Draw()
{
	engine_->DrawPostEffect("Test");
}