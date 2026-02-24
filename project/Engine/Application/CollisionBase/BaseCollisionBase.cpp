#include "BaseCollisionBase.h"
#include "GrowthEngine.h"

/// @brief 衝突対象の設定
/// @param hCollision 
void Engine::BaseCollisionBase::SetCollisionTarget(Collision3DHandle hCollision)const
{
	GrowthEngine::GetInstance()->SetCollisionTarget(hCollision_, hCollision);
}