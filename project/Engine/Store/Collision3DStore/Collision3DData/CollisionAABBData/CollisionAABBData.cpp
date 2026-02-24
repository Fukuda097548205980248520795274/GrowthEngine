#include "CollisionAABBData.h"
#include "Application/CollisionInstance/CollisionInstanceAABB/CollisionInstanceAABB.h"

/// @brief コンストラクタ
/// @param func 
Engine::CollisionAABBData::CollisionAABBData(std::function<void()> func, const std::string& name, Collision::Type type, Collision3DHandle hCollision)
	: CollisionBase3DData(func, name, type, hCollision) 
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision::AABB>();
	param_->center = Vector3(0.0f, 0.0f, 0.0f);
	param_->radius = Vector3(1.0f, 1.0f, 1.0f);
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::CollisionAABBData::CreateInstance()
{
	// パラメータの生成
	std::unique_ptr<Collision::AABB> instanceParam = std::make_unique<Collision::AABB>();
	instanceParam->center = param_->center;
	instanceParam->radius = param_->radius;

	std::unique_ptr<CollisionInstanceAABB> instance = std::make_unique<CollisionInstanceAABB>(instanceParam.get());
	CollisionInstanceAABB* pInstance = instance.get();

	instanceParams_.push_back(std::move(instanceParam));
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}