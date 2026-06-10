#include "Collision3DCapsuleData.h"
#include "Application/Collision3DInstance/Collision3DInstanceCapsule/Collision3DInstanceCapsule.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param func 
Engine::Collision3DCapsuleData::Collision3DCapsuleData(const std::string& name, Collision3D::Type type, Collision3DHandle hCollision)
	: Collision3DBaseData(name, type, hCollision)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// パラメータの生成と初期化
	param_ = std::make_unique<Collision3D::Capsule>();
	param_->start = Vector3(0.0f, 0.0f, 0.0f);
	param_->diff = Vector3(0.0f, 1.0f, 0.0f);
	param_->radius = 0.5f;
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::Collision3DCapsuleData::CreateInstance()
{
	std::unique_ptr<Collision3DInstanceCapsule> instance = std::make_unique<Collision3DInstanceCapsule>(param_.get());
	Collision3DInstanceCapsule* pInstance = instance.get();

	instanceTable_.push_back(std::move(instance));

	return pInstance;
}


/// @brief デバッグ用の線を描画する
void Engine::Collision3DCapsuleData::DebugDrawLine()
{
	for (auto& instance : instanceTable_)
	{
		// Sphereに型変換
		Collision3DInstanceCapsule* sphereInstance = static_cast<Collision3DInstanceCapsule*>(instance.get());

		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		if (sphereInstance->isCollision_)
			color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);


		Vector3 end = sphereInstance->param_->start + sphereInstance->param_->diff;
		engine_->DrawDebugLine3D(sphereInstance->param_->start, end, color);



		/*----------------
		    始点の大きさ
		----------------*/

		Vector3 startMin = sphereInstance->param_->start - Vector3(sphereInstance->param_->radius, sphereInstance->param_->radius, sphereInstance->param_->radius);
		Vector3 startMax = sphereInstance->param_->start + Vector3(sphereInstance->param_->radius, sphereInstance->param_->radius, sphereInstance->param_->radius);

		engine_->DrawDebugLine3D(startMin, Vector3(startMax.x, startMin.y, startMin.z), color);
		engine_->DrawDebugLine3D(startMin, Vector3(startMin.x, startMax.y, startMin.z), color);
		engine_->DrawDebugLine3D(startMin, Vector3(startMin.x, startMin.y, startMax.z), color);

		engine_->DrawDebugLine3D(startMax, Vector3(startMin.x, startMax.y, startMax.z), color);
		engine_->DrawDebugLine3D(startMax, Vector3(startMax.x, startMin.y, startMax.z), color);
		engine_->DrawDebugLine3D(startMax, Vector3(startMax.x, startMax.y, startMin.z), color);

		engine_->DrawDebugLine3D(Vector3(startMax.x, startMin.y, startMin.z), Vector3(startMax.x, startMax.y, startMin.z), color);
		engine_->DrawDebugLine3D(Vector3(startMin.x, startMin.y, startMax.z), Vector3(startMin.x, startMax.y, startMax.z), color);

		engine_->DrawDebugLine3D(Vector3(startMin.x, startMax.y, startMin.z), Vector3(startMax.x, startMax.y, startMin.z), color);
		engine_->DrawDebugLine3D(Vector3(startMin.x, startMax.y, startMin.z), Vector3(startMin.x, startMax.y, startMax.z), color);

		engine_->DrawDebugLine3D(Vector3(startMax.x, startMin.y, startMax.z), Vector3(startMin.x, startMin.y, startMax.z), color);
		engine_->DrawDebugLine3D(Vector3(startMax.x, startMin.y, startMax.z), Vector3(startMax.x, startMin.y, startMin.z), color);



		/*----------------
		    終点の大きさ
		----------------*/

		Vector3 endMin = end - Vector3(sphereInstance->param_->radius, sphereInstance->param_->radius, sphereInstance->param_->radius);
		Vector3 endMax = end + Vector3(sphereInstance->param_->radius, sphereInstance->param_->radius, sphereInstance->param_->radius);

		engine_->DrawDebugLine3D(endMin, Vector3(endMax.x, endMin.y, endMin.z), color);
		engine_->DrawDebugLine3D(endMin, Vector3(endMin.x, endMax.y, endMin.z), color);
		engine_->DrawDebugLine3D(endMin, Vector3(endMin.x, endMin.y, endMax.z), color);

		engine_->DrawDebugLine3D(endMax, Vector3(endMin.x, endMax.y, endMax.z), color);
		engine_->DrawDebugLine3D(endMax, Vector3(endMax.x, endMin.y, endMax.z), color);
		engine_->DrawDebugLine3D(endMax, Vector3(endMax.x, endMax.y, endMin.z), color);

		engine_->DrawDebugLine3D(Vector3(endMax.x, endMin.y, endMin.z), Vector3(endMax.x, endMax.y, endMin.z), color);
		engine_->DrawDebugLine3D(Vector3(endMin.x, endMin.y, endMax.z), Vector3(endMin.x, endMax.y, endMax.z), color);

		engine_->DrawDebugLine3D(Vector3(endMin.x, endMax.y, endMin.z), Vector3(endMax.x, endMax.y, endMin.z), color);
		engine_->DrawDebugLine3D(Vector3(endMin.x, endMax.y, endMin.z), Vector3(endMin.x, endMax.y, endMax.z), color);

		engine_->DrawDebugLine3D(Vector3(endMax.x, endMin.y, endMax.z), Vector3(endMin.x, endMin.y, endMax.z), color);
		engine_->DrawDebugLine3D(Vector3(endMax.x, endMin.y, endMax.z), Vector3(endMax.x, endMin.y, endMin.z), color);
	}
}