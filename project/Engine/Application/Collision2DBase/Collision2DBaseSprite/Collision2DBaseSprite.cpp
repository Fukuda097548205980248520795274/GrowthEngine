#include "Collision2DBaseSprite.h"
#include "Application/Collision2DInstance/Collision2DInstanceSprite/Collision2DInstanceSprite.h"
#include "GrowthEngine.h"

Collision2DBaseSprite::Collision2DBaseSprite(std::function<void()> func, const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision2D(func, name, Engine::Collision2D::Type::Sprite);
}

/// @brief インスタンスを作成する
/// @return 
Collision2DInstanceSprite* Collision2DBaseSprite::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision2DInstance<Collision2DInstanceSprite>(hCollision_);
}