#include "Collision2DCircleData.h"
#include "EngineObject/Collision2DInstance/Collision2DInstanceCircle/Collision2DInstanceCircle.h"

/// @brief コンストラクタ
/// @param name 
/// @param type 
/// @param hCollision 
Engine::Collision2DCircleData::Collision2DCircleData(const std::string& name, Collision2D::Type type, Collision2DHandle hCollision)
	: Collision2DBaseData(name, type, hCollision)
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

/// @brief デバッグ用描画処理
void Engine::Collision2DCircleData::DebugDrawLine()
{

}