#include "Collision2DInstanceCircle.h"

/// @brief コンストラクタ
/// @param param 
Collision2DInstanceCircle::Collision2DInstanceCircle(Engine::Collision2D::Circle* param)
{
	// 種類の設定
	type_ = Engine::Collision2D::Type::Circle;

	// パラメータの生成と初期化
	param_ = std::make_unique<Engine::Collision2D::Circle>();
	param_->center = param->center;
	param_->radius = param->radius;
}