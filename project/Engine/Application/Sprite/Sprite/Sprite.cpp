#include "Sprite.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param hTexture 
Sprite::Sprite(TextureHandle hTexture, const std::string& name) : name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 読み込む
	hSprite_ = engine_->LoadSprite(hTexture, name_);
}

/// @brief 描画処理
void Sprite::Draw()
{
	engine_->DrawSprite(hSprite_);
}