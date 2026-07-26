#include "Collision3DBaseOBB.h"
#include "Application/Collision3DInstance/Collision3DInstanceOBB/Collision3DInstanceOBB.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Collision3DBaseOBB::Collision3DBaseOBB(const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(name, Engine::Collision3D::Type::OBB);
}

/// @brief インスタンスを作成する
/// @return 
Collision3DInstanceOBB* Collision3DBaseOBB::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<Collision3DInstanceOBB>(hCollision_);
}