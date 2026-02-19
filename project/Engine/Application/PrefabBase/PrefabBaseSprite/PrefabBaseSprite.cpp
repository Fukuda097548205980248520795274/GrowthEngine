#include "PrefabBaseSprite.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param hTexture 
/// @param numInstance 
/// @param name 
PrefabBaseSprite::PrefabBaseSprite(TextureHandle hTexture, uint32_t numInstance, const std::string& name)
	: numInstance_(numInstance), name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 読み込む
	handle_ = engine_->LoadPrefabSprite(name_, numInstance, hTexture);

	// パラメータを取得する
	param_ = engine_->GetPrefabSpriteParam(handle_);
}

/// @brief インスタンスを生成する
[[nodiscard]]
PrefabInstanceSprite* PrefabBaseSprite::CreateInstance()
{
	return engine_->CreateSpriteInstance(handle_);
}

/// @brief 描画処理
void PrefabBaseSprite::Draw()
{
	engine_->DrawPrefabSprite(handle_);
}