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
	std::unique_ptr<Collision3DInstancePlane> instance = std::make_unique<Collision3DInstancePlane>(param_.get());
	Collision3DInstancePlane* pInstance = instance.get();

	instanceTable_.push_back(std::move(instance));

	return pInstance;
}