#include "Collision3DBaseRay.h"
#include "Application/Collision3DInstance/Collision3DInstanceRay/Collision3DInstanceRay.h"
#include "GrowthEngine.h"

Collision3DBaseRay::Collision3DBaseRay(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(func, name, Engine::Collision3D::Type::Ray);
}

/// @brief インスタンスを作成する
/// @return 
Collision3DInstanceRay* Collision3DBaseRay::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<Collision3DInstanceRay>(hCollision_);
}