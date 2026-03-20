#include "Sprite.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param hTexture 
Sprite::Sprite(TextureHandle hTexture, const std::string& name) : name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 読み込む
	hRender2D_ = engine_->LoadRender2D(hTexture, name_);

	// パラメータを取得する
	param_ = engine_->GetRender2DParam<Engine::Render2D::Sprite::Param>(hRender2D_);
}

/// @brief 描画処理
void Sprite::Draw()
{
	engine_->DrawRender2D(hRender2D_);
}