#include "Collision2DCircleData.h"
#include "Application/Collision2DInstance/Collision2DInstanceCircle/Collision2DInstanceCircle.h"

/// @brief コンストラクタ
/// @param func 
Engine::Collision2DCircleData::Collision2DCircleData(std::function<void()> func, const std::string& name, Collision2D::Type type, Collision2DHandle hCollision)
	: Collision2DBaseData(func, name, type, hCollision)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision2D::Circle>();
	param_->center = Vector2(0.0f, 0.0f);
	param_->radius = 1.0f;
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::Collision2DCircleData::CreateInstance()
{
	std::unique_ptr<Collision2DInstanceCircle> instance = std::make_unique<Collision2DInstanceCircle>(param_.get());
	Collision2DInstanceCircle* pInstance = instance.get();

	instanceTable_.push_back(std::move(instance));

	return pInstance;
}