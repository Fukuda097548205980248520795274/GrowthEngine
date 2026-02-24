#include "CollisionBaseLine.h"
#include "Application/CollisionInstance/CollisionInstanceLine/CollisionInstanceLine.h"
#include "GrowthEngine.h"

CollisionBaseLine::CollisionBaseLine(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(func, name, Engine::Collision::Type::Line);
}

/// @brief インスタンスを作成する
/// @return 
CollisionInstanceLine* CollisionBaseLine::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<CollisionInstanceLine>(hCollision_);
}