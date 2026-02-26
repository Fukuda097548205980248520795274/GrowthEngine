#include "Collision3DBaseAABB.h"
#include "Application/Collision3DInstance/Collision3DInstanceAABB/Collision3DInstanceAABB.h"
#include "GrowthEngine.h"

Collision3DBaseAABB::Collision3DBaseAABB(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(func, name, Engine::Collision3D::Type::AABB);
}

/// @brief インスタンスを作成する
/// @return 
Collision3DInstanceAABB* Collision3DBaseAABB::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<Collision3DInstanceAABB>(hCollision_);
}