#include "Collision3DBasePlane.h"
#include "Application/Collision3DInstance/Collision3DInstancePlane/Collision3DInstancePlane.h"
#include "GrowthEngine.h"

Collision3DBasePlane::Collision3DBasePlane(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(func, name, Engine::Collision3D::Type::Plane);
}

/// @brief インスタンスを作成する
/// @return 
Collision3DInstancePlane* Collision3DBasePlane::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<Collision3DInstancePlane>(hCollision_);
}