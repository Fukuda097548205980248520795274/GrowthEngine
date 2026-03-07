#include "Collision3DInstanceLine.h"

/// @brief コンストラクタ
/// @param param 
Collision3DInstanceLine::Collision3DInstanceLine(Engine::Collision3D::Line* param)
{
	// 形状の設定
	type_ = Engine::Collision3D::Type::Line;

	// パラメータの生成と初期化
	param_ = std::make_unique<Engine::Collision3D::Line>();
	param_->start = param->start;
	param_->diff = param->diff;
}