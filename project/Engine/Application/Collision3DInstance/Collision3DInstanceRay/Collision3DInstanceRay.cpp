#include "Collision3DInstanceRay.h"

/// @brief コンストラクタ
	/// @param param 
Collision3DInstanceRay::Collision3DInstanceRay(Engine::Collision3D::Ray* param)
{
	// 形状の設定
	type_ = Engine::Collision3D::Type::Ray;

	// パラメータの生成と初期あ
	param_ = std::make_unique<Engine::Collision3D::Ray>();
	param_->start = param->start;
	param_->diff = param->diff;
}