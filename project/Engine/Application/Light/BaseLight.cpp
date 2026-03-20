#include "BaseLight.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Engine::BaseLight::BaseLight(const std::string& name) : name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();
}