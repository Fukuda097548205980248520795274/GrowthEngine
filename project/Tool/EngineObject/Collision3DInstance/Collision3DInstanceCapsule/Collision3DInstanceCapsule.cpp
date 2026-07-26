#include "Collision3DInstanceCapsule.h"

/// @brief コンストラクタ
/// @param param 
Collision3DInstanceCapsule::Collision3DInstanceCapsule(Engine::Collision3D::Capsule* param)
{
	// 形状の設定
	type_ = Engine::Collision3D::Type::Capsule;

	// パラメータの生成と初期化
	param_ = std::make_unique<Engine::Collision3D::Capsule>();
	param_->start = param->start;
	param_->diff = param->diff;
	param_->radius = param->radius;
}