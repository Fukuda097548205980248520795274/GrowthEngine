#include "Collision3DBaseAABB.h"
#include "EngineObject/Collision3DInstance/Collision3DInstanceAABB/Collision3DInstanceAABB.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Collision3DBaseAABB::Collision3DBaseAABB(const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(name, Engine::Collision3D::Type::AABB);
}

/// @brief インスタンスを作成する
/// @return 
Collision3DInstanceAABB* Collision3DBaseAABB::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<Collision3DInstanceAABB>(hCollision_);
}