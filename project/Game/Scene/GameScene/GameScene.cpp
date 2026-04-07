#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	engine_->LoadCamera3D("MainCamera");

	// 地形のモデルを読み込む
	engine_->LoadPrimitiveStaticModel(engine_->LoadModel("./Assets/Models/terrain", "terrain.obj"), "Terrain");

	// ライトを読み込む
	engine_->LoadLight("DirectionalLight", Engine::Light::Type::Directional);

	// 輝度ベース輪郭抽出のポストエフェクトを読み込む
	engine_->LoadPostEffect("LuminanceBasedOutline", Engine::PostEffect::Type::LuminanceBasedOutline);
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

	// 輝度ベース輪郭抽出
	engine_->DrawPostEffect("LuminanceBasedOutline");
}