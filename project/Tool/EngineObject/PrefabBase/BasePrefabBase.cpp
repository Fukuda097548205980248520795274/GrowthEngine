#include "BasePrefabBase.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Engine::BasePrefabBase::BasePrefabBase(const std::string& name)
	: name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();
}