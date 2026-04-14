#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	engine_->LoadCamera3D("MainCamera");

	// モデルを読み込む
	engine_->LoadUVSphere(engine_->LoadTexture("./Assets/Textures/uvChecker.png"), "uv");

	// ライトを読み込む
	engine_->LoadLight("DirectionalLight", Engine::Light::Type::Directional);

	// ブルーム
	engine_->LoadPostEffect("Bloom", Engine::PostEffect::Type::Bloom);
}

/// @brief 更新処理
void GameScene::Update()
{
	
}

/// @brief 描画処理
void GameScene::Draw()
{
	// 地形を描画する
	engine_->DrawRender3D("uv");

	// ブルームを描画する
	engine_->DrawPostEffect(engine_->LoadPostEffect("Bloom", Engine::PostEffect::Type::Bloom));
}