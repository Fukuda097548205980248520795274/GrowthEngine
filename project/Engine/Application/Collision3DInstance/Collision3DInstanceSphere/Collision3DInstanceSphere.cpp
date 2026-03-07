#include "Collision3DInstanceSphere.h"

/// @brief コンストラクタ
/// @param param 
Collision3DInstanceSphere::Collision3DInstanceSphere(Engine::Collision3D::Sphere* param)
{
	// 形状の設定
	type_ = Engine::Collision3D::Type::Sphere;

	// パラメータの生成と初期化
	param_ = std::make_unique<Engine::Collision3D::Sphere>();
	param_->center = param->center;
	param_->radius = param->radius;
}