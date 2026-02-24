#include "CollisionBaseOBB.h"
#include "Application/CollisionInstance/CollisionInstanceOBB/CollisionInstanceOBB.h"
#include "GrowthEngine.h"

CollisionBaseOBB::CollisionBaseOBB(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(func, name, Engine::Collision::Type::OBB);
}

/// @brief インスタンスを作成する
/// @return 
CollisionInstanceOBB* CollisionBaseOBB::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<CollisionInstanceOBB>(hCollision_);
}