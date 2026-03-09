#include "Collision3DBaseRay.h"
#include "Application/Collision3DInstance/Collision3DInstanceRay/Collision3DInstanceRay.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Collision3DBaseRay::Collision3DBaseRay(const std::string& name) : BaseCollision3DBase(name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(name_, Engine::Collision3D::Type::Ray);
}

/// @brief インスタンスを作成する
/// @return 
Collision3DInstanceRay* Collision3DBaseRay::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<Collision3DInstanceRay>(hCollision_);
}