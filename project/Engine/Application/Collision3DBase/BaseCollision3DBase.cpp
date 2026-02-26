#include "BaseCollision3DBase.h"
#include "GrowthEngine.h"

/// @brief 衝突対象の設定
/// @param hCollision 
void Engine::BaseCollision3DBase::SetCollisionTarget(Collision3DHandle hCollision)const
{
	GrowthEngine::GetInstance()->SetCollision3DTarget(hCollision_, hCollision);
}