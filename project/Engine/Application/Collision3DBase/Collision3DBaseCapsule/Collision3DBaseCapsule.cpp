#include "Collision3DBaseCapsule.h"
#include "Application/Collision3DInstance/Collision3DInstanceCapsule/Collision3DInstanceCapsule.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Collision3DBaseCapsule::Collision3DBaseCapsule(const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(name, Engine::Collision3D::Type::Capsule);
}

/// @brief インスタンスを作成する
/// @return 
Collision3DInstanceCapsule* Collision3DBaseCapsule::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<Collision3DInstanceCapsule>(hCollision_);
}