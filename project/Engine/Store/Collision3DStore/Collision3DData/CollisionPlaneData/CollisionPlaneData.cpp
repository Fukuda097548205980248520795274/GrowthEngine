#include "CollisionPlaneData.h"
#include "Application/CollisionInstance/CollisionInstancePlane/CollisionInstancePlane.h"

/// @brief コンストラクタ
/// @param func 
Engine::CollisionPlaneData::CollisionPlaneData(std::function<void()> func, const std::string& name, Collision::Type type, Collision3DHandle hCollision)
	: CollisionBase3DData(func, name, type, hCollision)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision::Plane>();
	param_->normal = Vector3(0.0f, 1.0f, 0.0f);
	param_->distance = 0.0f;
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::CollisionPlaneData::CreateInstance()
{
	// パラメータの生成
	std::unique_ptr<Collision::Plane> instanceParam = std::make_unique<Collision::Plane>();
	instanceParam->normal = param_->normal;
	instanceParam->distance = param_->distance;

	std::unique_ptr<CollisionInstancePlane> instance = std::make_unique<CollisionInstancePlane>(instanceParam.get());
	CollisionInstancePlane* pInstance = instance.get();

	instanceParams_.push_back(std::move(instanceParam));
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}