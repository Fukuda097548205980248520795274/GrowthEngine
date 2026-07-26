#include "Collision3DInstanceAABB.h"

/// @brief コンストラクタ
	/// @param param 
Collision3DInstanceAABB::Collision3DInstanceAABB(Engine::Collision3D::AABB* param)
{
	// 形状を設定
	type_ = Engine::Collision3D::Type::AABB; 

	// パラメータの生成と初期化
	param_ = std::make_unique<Engine::Collision3D::AABB>();
	param_->center = param->center;
	param_->radius = param->radius;
}