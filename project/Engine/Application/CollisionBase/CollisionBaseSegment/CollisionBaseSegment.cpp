#include "CollisionBaseSegment.h"
#include "Application/CollisionInstance/CollisionInstanceSegment/CollisionInstanceSegment.h"
#include "GrowthEngine.h"

CollisionBaseSegment::CollisionBaseSegment(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(func, name, Engine::Collision3D::Type::Segment);
}

/// @brief インスタンスを作成する
/// @return 
CollisionInstanceSegment* CollisionBaseSegment::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<CollisionInstanceSegment>(hCollision_);
}