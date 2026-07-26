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

	/// @brief 描画処理
	void Draw() override;

	// パラメータ
	Engine::Prefab2D::Sprite::Base::Param* param_ = nullptr;


private:

	// 2Dプレハブハンドル
	Prefab2DHandle handle_ = 0;
};

