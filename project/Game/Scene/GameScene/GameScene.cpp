#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	engine_->LoadCamera3D("MainCamera");

	// 地形のモデルを読み込む
	engine_->LoadPrimitiveStaticModel(engine_->LoadModel("./Assets/Models/terrain", "terrain.obj"), "Terrain");

	engine_->LoadParticle3D("Particle3D", 1000, engine_->LoadModel("./Assets/Models/AnimatedCube", "AnimatedCube.gltf"));

	// ライトを読み込む
	engine_->LoadLight("DirectionalLight", Engine::Light::Type::Directional);

	// ラジアルブラーを読み込む
	engine_->LoadPostEffect("DOF", Engine::PostEffect::Type::DOF);
	engine_->LoadTexture("./Assets/Textures/noise0.png");
}

/// @brief 更新処理
void GameScene::Update()
{
	
}

/// @brief 描画処理
void GameScene::Draw()
{
	// 地形を描画する
	engine_->DrawRender3D("Terrain");

	// ラジアルブラー
	engine_->DrawPostEffect("DOF");
}