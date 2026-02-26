#include "Collision3DOBBData.h"
#include "Application/Collision3DInstance/Collision3DInstanceOBB/Collision3DInstanceOBB.h"

/// @brief コンストラクタ
/// @param func 
Engine::Collision3DOBBData::Collision3DOBBData(std::function<void()> func, const std::string& name, Collision3D::Type type, Collision3DHandle hCollision)
	: Collision3DBaseData(func, name, type, hCollision)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision3D::OBB>();
	param_->center = Vector3(0.0f, 0.0f, 0.0f);
	param_->radius = Vector3(1.0f, 1.0f, 1.0f);
	param_->oriented[0] = Vector3(1.0f, 0.0f, 0.0f);
	param_->oriented[1] = Vector3(0.0f, 1.0f, 0.0f);
	param_->oriented[2] = Vector3(0.0f, 0.0f, 1.0f);
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::Collision3DOBBData::CreateInstance()
{
	// パラメータの生成
	std::unique_ptr<Collision3D::OBB> instanceParam = std::make_unique<Collision3D::OBB>();
	instanceParam->center = param_->center;
	instanceParam->radius = param_->radius;
	instanceParam->oriented[0] = param_->oriented[0];
	instanceParam->oriented[1] = param_->oriented[1];
	instanceParam->oriented[2] = param_->oriented[2];

	std::unique_ptr<Collision3DInstanceOBB> instance = std::make_unique<Collision3DInstanceOBB>(instanceParam.get());
	Collision3DInstanceOBB* pInstance = instance.get();

	instanceParams_.push_back(std::move(instanceParam));
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}