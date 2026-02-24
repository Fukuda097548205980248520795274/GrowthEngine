#include "CollisionSegmentData.h"
#include "Application/CollisionInstance/CollisionInstanceSegment/CollisionInstanceSegment.h"

/// @brief コンストラクタ
/// @param func 
Engine::CollisionSegmentData::CollisionSegmentData(std::function<void()> func, const std::string& name, Collision::Type type, Collision3DHandle hCollision)
	: CollisionBase3DData(func, name, type, hCollision)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Collision::Segment>();
	param_->start = Vector3(0.0f, -1.0f, 0.0f);
	param_->diff = Vector3(0.0f, 2.0f, 0.0f);
}

/// @brief インスタンスを作成する
/// @return 
void* Engine::CollisionSegmentData::CreateInstance()
{
	// パラメータの生成
	std::unique_ptr<Collision::Segment> instanceParam = std::make_unique<Collision::Segment>();
	instanceParam->start = param_->start;
	instanceParam->diff = param_->diff;

	std::unique_ptr<CollisionInstanceSegment> instance = std::make_unique<CollisionInstanceSegment>(instanceParam.get());
	CollisionInstanceSegment* pInstance = instance.get();

	instanceParams_.push_back(std::move(instanceParam));
	instanceTable_.push_back(std::move(instance));

	return pInstance;
}