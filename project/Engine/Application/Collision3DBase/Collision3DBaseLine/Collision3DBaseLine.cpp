#include "Collision3DBaseLine.h"
#include "Application/Collision3DInstance/Collision3DInstanceLine/Collision3DInstanceLine.h"
#include "GrowthEngine.h"

Collision3DBaseLine::Collision3DBaseLine(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(func, name, Engine::Collision3D::Type::Line);
}

/// @brief インスタンスを作成する
/// @return 
Collision3DInstanceLine* Collision3DBaseLine::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<Collision3DInstanceLine>(hCollision_);
}