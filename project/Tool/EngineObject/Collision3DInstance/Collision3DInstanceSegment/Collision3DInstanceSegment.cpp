#include "Collision3DInstanceSegment.h"

/// @brief コンストラクタ
	/// @param param 
Collision3DInstanceSegment::Collision3DInstanceSegment(Engine::Collision3D::Segment* param) 
{
	// 形状の設定
	type_ = Engine::Collision3D::Type::Segment; 

	// パラメータの生成と初期化
	param_ = std::make_unique<Engine::Collision3D::Segment>();
	param_->start = param->start;
	param_->diff = param->diff;
}