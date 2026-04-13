#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	engine_->LoadCamera3D("MainCamera");

	// モデルを読み込む
	engine_->LoadPrimitiveSkinningModel(engine_->LoadModel("./Assets/Models/walk", "walk.gltf"),
		engine_->LoadAnimation("./Assets/Models/walk", "walk.gltf"), engine_->LoadSkeleton("./Assets/Models/walk", "walk.gltf"), "Walk");

	// ライトを読み込む
	engine_->LoadLight("DirectionalLight", Engine::Light::Type::Directional);

	// パーティクル
	engine_->LoadParticle3D("Particle_TEST", 1000, engine_->LoadModel("./Assets/Models/particlePlane", "particlePlane.obj"));
}

/// @brief 更新処理
void GameScene::Update()
{
	
}

/// @brief 描画処理
void GameScene::Draw()
{
	// 地形を描画する
	engine_->DrawRender3D("Walk");

	// パーティクルを描画する
	engine_->DrawParticle3D("Particle_TEST");
}