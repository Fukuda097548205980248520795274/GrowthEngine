#include "Collision2DBaseCircle.h"
#include "Application/Collision2DInstance/Collision2DInstanceCircle/Collision2DInstanceCircle.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Collision2DBaseCircle::Collision2DBaseCircle(const std::string& name) : BaseCollision2DBase(name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision2D(name_, Engine::Collision2D::Type::Sprite);
}

/// @brief インスタンスを作成する
/// @return 
Collision2DInstanceCircle* Collision2DBaseCircle::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision2DInstance<Collision2DInstanceCircle>(hCollision_);
}