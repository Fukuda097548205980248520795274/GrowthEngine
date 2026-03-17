#pragma once
#include "../BasePrefabBase.h"

class PrefabInstanceSprite;

class PrefabBaseSprite : public Engine::BasePrefabBase
{
public:

	/// @brief コンストラクタ
	/// @param hTexture 
	/// @param numInstance 
	/// @param name 
	PrefabBaseSprite(TextureHandle hTexture, uint32_t numInstance, const std::string& name);

	/// @brief インスタンスを生成する
	[[nodiscard]]
	PrefabInstanceSprite* CreateInstance();

	// パラメータ
	Engine::Prefab::Sprite::Base::Param* param_ = nullptr;


private:

	// プレハブスプライトハンドル
	PrefabSpriteHandle handle_ = 0;
};

