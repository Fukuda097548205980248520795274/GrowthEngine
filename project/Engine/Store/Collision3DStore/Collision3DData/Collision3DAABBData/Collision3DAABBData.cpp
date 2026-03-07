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

	std::unique_ptr<Collision3DInstanceAABB> instance = std::make_unique<Collision3DInstanceAABB>(param_.get());
	Collision3DInstanceAABB* pInstance = instance.get();

	instanceTable_.push_back(std::move(instance));

	return pInstance;
}