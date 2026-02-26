#include "Collision3DAABBData.h"
#include "Application/Collision3DInstance/Collision3DInstanceAABB/Collision3DInstanceAABB.h"

/// @brief コンストラクタ
/// @param func 
Engine::Collision3DAABBData::Collision3DAABBData(std::function<void()> func, const std::string& name, Collision3D::Type type, Collision3DHandle hCollision)
	: Collision3DBaseData(func, name, type, hCollision)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision3D::AABB>();
	param_->center = Vector3(0.0f, 0.0f, 0.0f);
	param_->radius = Vector3(1.0f, 1.0f, 1.0f);
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::Collision3DAABBData::CreateInstance()
{
	// パラメータの生成
	std::unique_ptr<Collision3D::AABB> instanceParam = std::make_unique<Collision3D::AABB>();
	instanceParam->center = param_->center;
	instanceParam->radius = param_->radius;

	std::unique_ptr<Collision3DInstanceAABB> instance = std::make_unique<Collision3DInstanceAABB>(instanceParam.get());
	Collision3DInstanceAABB* pInstance = instance.get();

	instanceParams_.push_back(std::move(instanceParam));
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}