#include "CollisionLineData.h"
#include "Application/CollisionInstance/CollisionInstanceLine/CollisionInstanceLine.h"

/// @brief コンストラクタ
/// @param func 
Engine::CollisionLineData::CollisionLineData(std::function<void()> func, const std::string& name, Collision::Type type, Collision3DHandle hCollision)
	: CollisionBase3DData(func, name, type, hCollision)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision::Line>();
	param_->start = Vector3(0.0f, -1.0f, 0.0f);
	param_->diff = Vector3(0.0f, 2.0f, 0.0f);
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::CollisionLineData::CreateInstance()
{
	// パラメータの生成
	std::unique_ptr<Collision::Line> instanceParam = std::make_unique<Collision::Line>();
	instanceParam->start = param_->start;
	instanceParam->diff = param_->diff;

	std::unique_ptr<CollisionInstanceLine> instance = std::make_unique<CollisionInstanceLine>(instanceParam.get());
	CollisionInstanceLine* pInstance = instance.get();

	instanceParams_.push_back(std::move(instanceParam));
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}