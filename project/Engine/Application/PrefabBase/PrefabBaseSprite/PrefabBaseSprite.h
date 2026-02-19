#pragma once
#include "Handle/Handle.h"
#include "Application/PrefabInstance/PrefabInstanceSprite/PrefabInstanceSprite.h"
#include <string>
#include <list>
#include <memory>

class GrowthEngine;

class PrefabBaseSprite
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
	void Draw();

	// パラメータ
	Engine::Prefab::Sprite::Base::Param* param_ = nullptr;


private:

	// エンジン
	const GrowthEngine* engine_ = nullptr;

	// インスタンス数
	uint32_t numInstance_ = 0;

	// 名前
	std::string name_{};

	// プレハブスプライトハンドル
	PrefabSpriteHandle handle_ = 0;
};

