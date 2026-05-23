#include "TitleScene.h"

/// @brief 初期化
void TitleScene::Initialize()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	engine_->LoadLight("DirectionalLight", Engine::Light::Type::Directional);

	prefabTube_ = std::make_unique<PrefabBaseTube>(engine_->LoadTexture("./Assets/Textures/uvChecker.png"), 32, "PrefabTube");

	instanceTube0_ = prefabTube_->CreateInstance();
	instanceTube0_->param_.transform.translate = { -2.0f, 0.0f, 0.0f };
	instanceTube0_->param_.size.height = 2.0f;

	instanceTube1_ = prefabTube_->CreateInstance();
	instanceTube1_->param_.transform.translate = { 0.0f, 0.0f, 0.0f };
	instanceTube1_->param_.size.radiusTop = 0.25f;

	instanceTube2_ = prefabTube_->CreateInstance();
	instanceTube2_->param_.transform.translate = { 2.0f, 0.0f, 0.0f };
	instanceTube2_->param_.size.radiusBottom = 0.25f;
}

/// @brief 更新処理
void TitleScene::Update()
{
	
}

/// @brief 描画処理
void TitleScene::Draw()
{
	instanceTube0_->Draw();
	instanceTube1_->Draw();
	instanceTube2_->Draw();
	prefabTube_->Draw();
}