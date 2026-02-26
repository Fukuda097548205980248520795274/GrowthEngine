#include "Collision3DPlaneData.h"
#include "Application/Collision3DInstance/Collision3DInstancePlane/Collision3DInstancePlane.h"

/// @brief コンストラクタ
/// @param func 
Engine::Collision3DPlaneData::Collision3DPlaneData(std::function<void()> func, const std::string& name, Collision3D::Type type, Collision3DHandle hCollision)
	: Collision3DBaseData(func, name, type, hCollision)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision3D::Plane>();
	param_->normal = Vector3(0.0f, 1.0f, 0.0f);
	param_->distance = 0.0f;
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::Collision3DPlaneData::CreateInstance()
{
	// パラメータの生成
	std::unique_ptr<Collision3D::Plane> instanceParam = std::make_unique<Collision3D::Plane>();
	instanceParam->normal = param_->normal;
	instanceParam->distance = param_->distance;

	std::unique_ptr<Collision3DInstancePlane> instance = std::make_unique<Collision3DInstancePlane>(instanceParam.get());
	Collision3DInstancePlane* pInstance = instance.get();

	instanceParams_.push_back(std::move(instanceParam));
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}