#include "CollisionBaseAABB.h"
#include "Application/CollisionInstance/CollisionInstanceAABB/CollisionInstanceAABB.h"
#include "GrowthEngine.h"

CollisionBaseAABB::CollisionBaseAABB(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(func, name, Engine::Collision::Type::AABB);
}

/// @brief インスタンスを作成する
/// @return 
CollisionInstanceAABB* CollisionBaseAABB::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<CollisionInstanceAABB>(hCollision_);
}

/// @brief 衝突対象の設定
/// @param hCollision 
void CollisionBaseAABB::SetTargetCollision(Collision3DHandle hCollision)
{
	GrowthEngine::GetInstance()->SetCollisionTarget(hCollision_, hCollision);
}