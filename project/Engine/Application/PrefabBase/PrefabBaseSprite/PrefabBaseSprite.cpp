#include "PrefabBaseSprite.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param hTexture 
/// @param numInstance 
/// @param name 
PrefabBaseSprite::PrefabBaseSprite(TextureHandle hTexture, uint32_t numInstance, const std::string& name) 
	: BasePrefabBase(name)
{
	// 読み込む
	handle_ = engine_->LoadPrefabSprite(name_, numInstance, hTexture);

	// パラメータを取得する
	param_ = engine_->GetPrefab2DParam<Engine::Prefab2D::Sprite::Base::Param>(handle_);
}

/// @brief インスタンスを生成する
[[nodiscard]]
PrefabInstanceSprite* PrefabBaseSprite::CreateInstance()
{
	return engine_->CreatePrefab2DInstance<PrefabInstanceSprite>(handle_);
}