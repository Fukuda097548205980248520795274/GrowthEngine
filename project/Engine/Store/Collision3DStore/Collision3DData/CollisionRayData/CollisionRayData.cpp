#include "CollisionRayData.h"
#include "Application/CollisionInstance/CollisionInstanceRay/CollisionInstanceRay.h"

/// @brief コンストラクタ
/// @param func 
Engine::CollisionRayData::CollisionRayData(std::function<void()> func, const std::string& name, Collision::Type type, Collision3DHandle hCollision)
	: CollisionBase3DData(func, name, type, hCollision)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision::Ray>();
	param_->start = Vector3(0.0f, -1.0f, 0.0f);
	param_->diff = Vector3(0.0f, 2.0f, 0.0f);
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::CollisionRayData::CreateInstance()
{
	// パラメータの生成
	std::unique_ptr<Collision::Ray> instanceParam = std::make_unique<Collision::Ray>();
	instanceParam->start = param_->start;
	instanceParam->diff = param_->diff;

	std::unique_ptr<CollisionInstanceRay> instance = std::make_unique<CollisionInstanceRay>(instanceParam.get());
	CollisionInstanceRay* pInstance = instance.get();

	instanceParams_.push_back(std::move(instanceParam));
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}