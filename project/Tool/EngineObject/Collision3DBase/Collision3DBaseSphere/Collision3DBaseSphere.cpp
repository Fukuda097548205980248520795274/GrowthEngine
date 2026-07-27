#include "Collision3DBaseSphere.h"
#include "EngineObject/Collision3DInstance/Collision3DInstanceSphere/Collision3DInstanceSphere.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Collision3DBaseSphere::Collision3DBaseSphere(const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(name, Engine::Collision3D::Type::Sphere);
}

/// @brief インスタンスを作成する
/// @return 
Collision3DInstanceSphere* Collision3DBaseSphere::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<Collision3DInstanceSphere>(hCollision_);
}