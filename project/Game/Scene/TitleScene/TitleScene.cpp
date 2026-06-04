#include "TitleScene.h"

/// @brief 初期化
void TitleScene::Initialize()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	engine_->LoadLight("DirectionalLight", Engine::Light::Type::Directional);

	engine_->LoadPostEffect("GaussianFilter", Engine::PostEffect::Type::GaussianFilter);

	engine_->LoadRing(engine_->LoadTexture("./Assets/Textures/uvChecker.png"), "Test_Ring");
	engine_->LoadCylinder(engine_->LoadTexture("./Assets/Textures/uvChecker.png"), "Test_Cylinder");
	engine_->LoadUVSphere(engine_->LoadTexture("./Assets/Textures/uvChecker.png"), "Test_UVSphere");
}

/// @brief 更新処理
void TitleScene::Update()
{
	
}

/// @brief 描画処理
void TitleScene::Draw()
{
	engine_->DrawDebugTriangle3D(
		Vector3{ 0.0f, 0.0f, 0.0f },
		Vector3{ 1.0f, 0.0f, 0.0f },
		Vector3{ 0.0f, 1.0f, 0.0f },
		Vector4{ 1.0f, 0.0f, 0.0f, 1.0f }
	);

	engine_->DrawRender3D("Test_Ring");
	engine_->DrawRender3D("Test_Cylinder");

	engine_->DrawRender3D("Test_UVSphere");

	engine_->DrawPostEffect("GaussianFilter");
}