#include "Collision3DBaseSegment.h"
#include "EngineObject/Collision3DInstance/Collision3DInstanceSegment/Collision3DInstanceSegment.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Collision3DBaseSegment::Collision3DBaseSegment(const std::string& name)
{
	// 読み込む
	hCollision_ = GrowthEngine::GetInstance()->LoadCollision3D(name, Engine::Collision3D::Type::Segment);
}

/// @brief インスタンスを作成する
/// @return 
Collision3DInstanceSegment* Collision3DBaseSegment::CreateInstance()
{
	return GrowthEngine::GetInstance()->CreateCollision3DInstance<Collision3DInstanceSegment>(hCollision_);
}