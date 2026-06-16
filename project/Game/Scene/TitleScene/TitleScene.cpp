#include "TitleScene.h"

/// @brief 初期化
void TitleScene::Initialize()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	engine_->LoadRing(engine_->LoadTexture("./Assets/Textures/uvChecker.png"), "TitleRing");
	engine_->LoadPostEffect("Outline_Luminance", Engine::PostEffect::Type::LuminanceBasedOutline);
	engine_->LoadPostEffect("Outline_Depth", Engine::PostEffect::Type::DepthBasedOutline);
}

/// @brief 更新処理
void TitleScene::Update()
{
	
}

/// @brief 描画処理
void TitleScene::Draw()
{
	engine_->DrawRender3D("TitleRing");
	engine_->DrawPostEffect("Outline_Luminance");
	engine_->DrawPostEffect("Outline_Depth");
}