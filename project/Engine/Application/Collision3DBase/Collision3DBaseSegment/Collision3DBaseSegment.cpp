#include "Collision3DBaseSegment.h"
#include "Application/Collision3DInstance/Collision3DInstanceSegment/Collision3DInstanceSegment.h"
#include "GrowthEngine.h"

Collision3DBaseSegment::Collision3DBaseSegment(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(func, name, Engine::Collision3D::Type::Segment);
}

/// @brief インスタンスを作成する
/// @return 
Collision3DInstanceSegment* Collision3DBaseSegment::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<Collision3DInstanceSegment>(hCollision_);
}