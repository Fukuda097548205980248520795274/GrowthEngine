#include "BaseLight.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Engine::BaseLight::BaseLight(const std::string& name) : name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();
}

/// @brief 設置する
void Engine::BaseLight::Set()
{
	engine_->SetLight(hLight_);
}