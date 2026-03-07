#include "Collision3DInstanceOBB.h"

/// @brief コンストラクタ
	/// @param param 
Collision3DInstanceOBB::Collision3DInstanceOBB(Engine::Collision3D::OBB* param)
{
	// 形状の設定
	type_ = Engine::Collision3D::Type::OBB;

	// パラメータの生成と初期化
	param_ = std::make_unique<Engine::Collision3D::OBB>();
	param_->center = param->center;
	param_->oriented[0] = param->oriented[0];
	param_->oriented[1] = param->oriented[1];
	param_->oriented[2] = param->oriented[2];
	param_->radius = param->radius;
}