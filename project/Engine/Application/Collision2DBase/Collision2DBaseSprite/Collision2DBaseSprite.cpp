#include "Collision2DBaseSprite.h"
#include "Application/Collision2DInstance/Collision2DInstanceSprite/Collision2DInstanceSprite.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Collision2DBaseSprite::Collision2DBaseSprite(const std::string& name) : BaseCollision2DBase(name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision2D(name_, Engine::Collision2D::Type::Sprite);
}

/// @brief インスタンスを作成する
/// @return 
Collision2DInstanceSprite* Collision2DBaseSprite::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision2DInstance<Collision2DInstanceSprite>(hCollision_);
}