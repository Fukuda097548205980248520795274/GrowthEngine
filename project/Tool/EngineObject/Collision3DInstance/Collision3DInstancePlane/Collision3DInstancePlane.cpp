#include "Collision3DInstancePlane.h"

/// @brief コンストラクタ
	/// @param param 
Collision3DInstancePlane::Collision3DInstancePlane(Engine::Collision3D::Plane* param)
{
	// 形状の設定
	type_ = Engine::Collision3D::Type::Plane; 

	// パラメータの生成と初期化
	param_ = std::make_unique<Engine::Collision3D::Plane>();
	param_->distance = param->distance;
	param_->normal = param->normal;
}