#include "TitleScene.h"

/// @brief 初期化
void TitleScene::Initialize()
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	engine_->LoadParticle3D("Test_Particle", 1000, 10, engine_->LoadModel("./Assets/Models/spark", "spark.obj"));
}

/// @brief 更新処理
void TitleScene::Update()
{
	Emitter3D emitter("Test_Particle");
	emitter.param_->position = emitterPosition_;
	emitter.Emit();
}

/// @brief 描画処理
void TitleScene::Draw()
{
	engine_->DrawParticle3D("Test_Particle");
}