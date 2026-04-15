#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	engine_->LoadCamera3D("MainCamera");

	engine_->LoadPrimitiveSkinningModel(
		engine_->LoadModel("./Assets/Models/bone", "bone.gltf"),
		engine_->LoadAnimation("./Assets/Models/bone", "bone.gltf"),
		engine_->LoadSkeleton("./Assets/Models/bone", "bone.gltf"), "TEST");

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
	engine_->DrawRender3D("TEST");

	// ブルームを描画する
	engine_->DrawPostEffect(engine_->LoadPostEffect("Bloom", Engine::PostEffect::Type::Bloom));
}