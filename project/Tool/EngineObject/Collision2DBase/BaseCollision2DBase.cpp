#include "BaseCollision2DBase.h"
#include "GrowthEngine.h"

/// @brief 衝突対象の設定
/// @param hCollision 
void Engine::BaseCollision2DBase::SetCollisionTarget(Collision2DHandle hCollision)const
{
	GrowthEngine::GetInstance()->SetCollision2DTarget(hCollision_, hCollision);
}