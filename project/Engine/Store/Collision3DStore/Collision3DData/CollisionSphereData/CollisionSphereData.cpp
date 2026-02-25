#include "CollisionSphereData.h"
#include "Application/CollisionInstance/CollisionInstanceSphere/CollisionInstanceSphere.h"

/// @brief コンストラクタ
/// @param func 
Engine::CollisionSphereData::CollisionSphereData(std::function<void()> func, const std::string& name, Collision3D::Type type, Collision3DHandle hCollision)
	: CollisionBase3DData(func, name, type, hCollision)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision3D::Sphere>();
	param_->center = Vector3(0.0f, 0.0f, 0.0f);
	param_->radius = 1.0f;
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::CollisionSphereData::CreateInstance()
{
	// パラメータの生成
	std::unique_ptr<Collision3D::Sphere> instanceParam = std::make_unique<Collision3D::Sphere>();
	instanceParam->center = param_->center;
	instanceParam->radius = param_->radius;

	std::unique_ptr<CollisionInstanceSphere> instance = std::make_unique<CollisionInstanceSphere>(instanceParam.get());
	CollisionInstanceSphere* pInstance = instance.get();

	instanceParams_.push_back(std::move(instanceParam));
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}