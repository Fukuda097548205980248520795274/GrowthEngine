#include "Collision3DAABBData.h"
#include "EngineObject/Collision3DInstance/Collision3DInstanceAABB/Collision3DInstanceAABB.h"

#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param func 
Engine::Collision3DAABBData::Collision3DAABBData(const std::string& name, Collision3D::Type type, Collision3DHandle hCollision)
	: Collision3DBaseData(name, type, hCollision)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

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

/// @brief デバッグ用の線を描画する
void Engine::Collision3DAABBData::DebugDrawLine()
{
	for (auto& instance : instanceTable_)
	{
		// AABBに型変換
		Collision3DInstanceAABB* aabbInstance = static_cast<Collision3DInstanceAABB*>(instance.get());

		Vector3 min = aabbInstance->param_->center - aabbInstance->param_->radius;
		Vector3 max = aabbInstance->param_->center + aabbInstance->param_->radius;

		Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		if (aabbInstance->isCollision_)
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