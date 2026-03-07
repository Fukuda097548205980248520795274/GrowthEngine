#include "Collision2DSpriteData.h"
#include "Application/Collision2DInstance/Collision2DInstanceSprite/Collision2DInstanceSprite.h"

/// @brief コンストラクタ
/// @param func 
Engine::Collision2DSpriteData::Collision2DSpriteData(std::function<void()> func, const std::string& name, Collision2D::Type type, Collision2DHandle hCollision)
	: Collision2DBaseData(func, name, type, hCollision)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision2D::Sprite>();
	param_->center = Vector2(0.0f, 0.0f);
	param_->radius = Vector2(1.0f, 1.0f);
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::Collision2DSpriteData::CreateInstance()
{
	std::unique_ptr<Collision2DInstanceSprite> instance = std::make_unique<Collision2DInstanceSprite>(param_.get());
	Collision2DInstanceSprite* pInstance = instance.get();

	instanceTable_.push_back(std::move(instance));

	return pInstance;
}