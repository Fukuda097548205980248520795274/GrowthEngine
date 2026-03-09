#include "Collision2DSpriteData.h"
#include "Application/Collision2DInstance/Collision2DInstanceSprite/Collision2DInstanceSprite.h"
#include "Math/Vector/Vector4/Vector4.h"

#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param func 
Engine::Collision2DSpriteData::Collision2DSpriteData(const std::string& name, Collision2D::Type type, Collision2DHandle hCollision)
	: Collision2DBaseData(name, type, hCollision)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

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

/// @brief デバッグ用描画処理
void Engine::Collision2DSpriteData::DebugDrawLine()
{
	for (auto& instance : instanceTable_)
	{
		Collision2DInstanceSprite* spriteInstance = static_cast<Collision2DInstanceSprite*>(instance.get());

		Vector2 leftTop = spriteInstance->param_->center - spriteInstance->param_->radius;
		Vector2 rightBottom = spriteInstance->param_->center + spriteInstance->param_->radius;
		Vector2 rightTop = Vector2(rightBottom.x, leftTop.y);
		Vector2 leftBottom = Vector2(leftTop.x, rightBottom.y);

		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		if (spriteInstance->isCollision_)
			color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);


		engine_->DrawDebugLine2D(leftTop, rightTop, color);
		engine_->DrawDebugLine2D(rightTop, rightBottom, color);
		engine_->DrawDebugLine2D(rightBottom, leftBottom, color);
		engine_->DrawDebugLine2D(leftBottom, leftTop, color);
	}
}