#include "Collision3DSphereData.h"
#include "EngineObject/Collision3DInstance/Collision3DInstanceSphere/Collision3DInstanceSphere.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param func 
Engine::Collision3DSphereData::Collision3DSphereData(const std::string& name, Collision3D::Type type, Collision3DHandle hCollision)
	: Collision3DBaseData(name, type, hCollision)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

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


/// @brief デバッグ用の線を描画する
void Engine::Collision3DSphereData::DebugDrawLine()
{
	for (auto& instance : instanceTable_)
	{
		// Sphereに型変換
		Collision3DInstanceSphere* sphereInstance = static_cast<Collision3DInstanceSphere*>(instance.get());

		Vector3 min = sphereInstance->param_->center - Vector3(sphereInstance->param_->radius , sphereInstance->param_->radius, sphereInstance->param_->radius);
		Vector3 max = sphereInstance->param_->center + Vector3(sphereInstance->param_->radius , sphereInstance->param_->radius, sphereInstance->param_->radius);

		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		if (sphereInstance->isCollision_)
			color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

		engine_->DrawDebugLine3D(min, Vector3(max.x, min.y, min.z), color);
		engine_->DrawDebugLine3D(min, Vector3(min.x, max.y, min.z), color);
		engine_->DrawDebugLine3D(min, Vector3(min.x, min.y, max.z), color);

		engine_->DrawDebugLine3D(max, Vector3(min.x, max.y, max.z), color);
		engine_->DrawDebugLine3D(max, Vector3(max.x, min.y, max.z), color);
		engine_->DrawDebugLine3D(max, Vector3(max.x, max.y, min.z), color);

		engine_->DrawDebugLine3D(Vector3(max.x, min.y, min.z), Vector3(max.x, max.y, min.z), color);
		engine_->DrawDebugLine3D(Vector3(min.x, min.y, max.z), Vector3(min.x, max.y, max.z), color);

		engine_->DrawDebugLine3D(Vector3(min.x, max.y, min.z), Vector3(max.x, max.y, min.z), color);
		engine_->DrawDebugLine3D(Vector3(min.x, max.y, min.z), Vector3(min.x, max.y, max.z), color);

		engine_->DrawDebugLine3D(Vector3(max.x, min.y, max.z), Vector3(min.x, min.y, max.z), color);
		engine_->DrawDebugLine3D(Vector3(max.x, min.y, max.z), Vector3(max.x, min.y, min.z), color);
	}
}