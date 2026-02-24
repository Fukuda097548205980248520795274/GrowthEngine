#include "CollisionBaseRay.h"
#include "Application/CollisionInstance/CollisionInstanceRay/CollisionInstanceRay.h"
#include "GrowthEngine.h"

CollisionBaseRay::CollisionBaseRay(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(func, name, Engine::Collision::Type::Ray);
}

/// @brief インスタンスを作成する
/// @return 
CollisionInstanceRay* CollisionBaseRay::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<CollisionInstanceRay>(hCollision_);
}