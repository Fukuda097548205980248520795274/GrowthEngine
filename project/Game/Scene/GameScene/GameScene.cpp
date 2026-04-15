#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	engine_->LoadCamera3D("MainCamera");

	// モデルを読み込む
	engine_->LoadParticle3D("particle", 1000, engine_->LoadModel("./Assets/Models/AnimatedCube", "AnimatedCube.gltf"));

	// ライトを読み込む
	engine_->LoadLight("DirectionalLight", Engine::Light::Type::Directional);

	// ブルーム
	engine_->LoadPostEffect("Bloom", Engine::PostEffect::Type::Bloom);

	engine_->LoadSound("TEST_Sound", engine_->LoadAudio("./Assets/Sounds/se/button.mp3"));
}

/// @brief 更新処理
void GameScene::Update()
{
	
}

/// @brief 描画処理
void GameScene::Draw()
{
	// 地形を描画する
	engine_->DrawParticle3D("particle");

	// ブルームを描画する
	engine_->DrawPostEffect(engine_->LoadPostEffect("Bloom", Engine::PostEffect::Type::Bloom));
}