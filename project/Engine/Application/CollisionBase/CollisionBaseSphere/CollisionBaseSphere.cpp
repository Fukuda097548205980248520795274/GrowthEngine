#include "CollisionBaseSphere.h"
#include "Application/CollisionInstance/CollisionInstanceSphere/CollisionInstanceSphere.h"
#include "GrowthEngine.h"

CollisionBaseSphere::CollisionBaseSphere(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(func, name, Engine::Collision::Type::Sphere);
}

/// @brief インスタンスを作成する
/// @return 
CollisionInstanceSphere* CollisionBaseSphere::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<CollisionInstanceSphere>(hCollision_);
}