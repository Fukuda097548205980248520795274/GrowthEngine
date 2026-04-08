#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	engine_->LoadCamera3D("MainCamera");

	// 地形のモデルを読み込む
	engine_->LoadPrimitiveStaticModel(engine_->LoadModel("./Assets/Models/terrain", "terrain.obj"), "Terrain");

	// ライトを読み込む
	engine_->LoadLight("DirectionalLight", Engine::Light::Type::Directional);

	// ラジアルブラーを読み込む
	engine_->LoadPostEffect("DOF", Engine::PostEffect::Type::DOF);
	engine_->LoadTexture("./Assets/Textures/noise0.png");
	auto param = engine_->GetPostEffectParam<Engine::PostEffect::DOF>("DOF");
	param->focusRange = 3.0f;
}

/// @brief 更新処理
void GameScene::Update()
{
	auto param = engine_->GetPostEffectParam<Engine::PostEffect::DOF>("DOF");
	param->focusDistance += focusVel_;

	if(param->focusDistance > 40.0f || param->focusDistance <= 0.0f)
	{
		focusVel_ *= -1.0f;
	}
}

/// @brief 描画処理
void GameScene::Draw()
{
	// 地形を描画する
	engine_->DrawRender3D("Terrain");

	// ラジアルブラー
	engine_->DrawPostEffect("DOF");
}