#include "Collision3DBasePlane.h"
#include "Application/Collision3DInstance/Collision3DInstancePlane/Collision3DInstancePlane.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Collision3DBasePlane::Collision3DBasePlane(const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(name, Engine::Collision3D::Type::Plane);
}

/// @brief インスタンスを作成する
/// @return 
Collision3DInstancePlane* Collision3DBasePlane::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<Collision3DInstancePlane>(hCollision_);
}