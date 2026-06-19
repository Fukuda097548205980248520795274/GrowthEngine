#include "Trail3D.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param maxLifeTime 
/// @param hTexture 
Trail3D::Trail3D(const std::string& name, float maxLifeTime, TextureHandle hTexture)
	: name_(name)
{
	// エンジンを取得する
	engine_ = GrowthEngine::GetInstance();

	// 3Dトレイルを読み込む
	hTrail_ = engine_->LoadTrail(name, maxLifeTime, hTexture);

	// パラメータを取得する
	param_ = engine_->GetTrailParam(name);
}

/// @brief 描画処理
void Trail3D::Draw()
{
	// トレイルを描画する
	engine_->DrawTrail(name_);
}