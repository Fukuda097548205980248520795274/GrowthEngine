#include "Collision3DSphereData.h"
#include "Application/Collision3DInstance/Collision3DInstanceSphere/Collision3DInstanceSphere.h"

/// @brief コンストラクタ
/// @param func 
Engine::Collision3DSphereData::Collision3DSphereData(std::function<void()> func, const std::string& name, Collision3D::Type type, Collision3DHandle hCollision)
	: Collision3DBaseData(func, name, type, hCollision)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision3D::Sphere>();
	param_->center = Vector3(0.0f, 0.0f, 0.0f);
	param_->radius = 1.0f;
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::Collision3DSphereData::CreateInstance()
{
	std::unique_ptr<Collision3DInstanceSphere> instance = std::make_unique<Collision3DInstanceSphere>(param_.get());
	Collision3DInstanceSphere* pInstance = instance.get();

	instanceTable_.push_back(std::move(instance));

	return pInstance;
}