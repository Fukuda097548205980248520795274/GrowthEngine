#include "CollisionBasePlane.h"
#include "Application/CollisionInstance/CollisionInstancePlane/CollisionInstancePlane.h"
#include "GrowthEngine.h"

CollisionBasePlane::CollisionBasePlane(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(func, name, Engine::Collision::Type::Plane);
}

/// @brief インスタンスを作成する
/// @return 
CollisionInstancePlane* CollisionBasePlane::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<CollisionInstancePlane>(hCollision_);
}