#include "Collision2DInstanceSprite.h"

/// @brief コンストラクタ
/// @param param 
Collision2DInstanceSprite::Collision2DInstanceSprite(Engine::Collision2D::Sprite* param) 
{
	// 種類の設定
	type_ = Engine::Collision2D::Type::Sprite; 

	// パラメータの生成と初期化
	param_ = std::make_unique<Engine::Collision2D::Sprite>();
	param_->center = param->center;
	param_->radius = param->radius;
}