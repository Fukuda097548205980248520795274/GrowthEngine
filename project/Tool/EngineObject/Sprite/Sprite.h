#pragma once
#include "Handle/Handle.h"
#include "Data/Render2DData/Render2DData.h"
#include <memory>
#include <string>

/// @brief エンジン
class GrowthEngine;

/// @brief ワールドトランスフォーム2D
class WorldTransform2D;

/// @brief スプライト
class Sprite
{
public:

	/// @brief コンストラクタ
	/// @param hTexture 
	/// @param name 
	Sprite(TextureHandle hTexture, const std::string& name);

	/// @brief 親を設定する
	/// @param parent 
	void SetParent(WorldTransform2D* parent);

	/// @brief 描画処理
	void Draw();

	/// @brief パラメータ
	Engine::Render2D::Sprite::Param* param_ = nullptr;


private:

	/// @brief エンジン
	const GrowthEngine* engine_ = nullptr;

	// ハンドル
	Render2DHandle hRender2D_ = 0;

	// 名前
	std::string name_{};
};

