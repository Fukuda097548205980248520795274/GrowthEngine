#include "LightDirectional.h"
#include "GrowthEngine.h"
#include "Store/LightStore/LightData/DirectionalLightData/DirectionalLightData.h"

/// @brief コンストラクタ
/// @param name 
LightDirectional::LightDirectional(const std::string& name) 
	: name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// ライトを読み込む
	handle_ = engine_->LoadLight(name, Engine::Light::Type::Directional);

	auto param = engine_->GetLightParam<Engine::Light::DirectionalLightParam>(handle_);
	param_ = param;
}

/// @brief 設置
void LightDirectional::Set()
{
	engine_->SetLight(handle_, Engine::Light::Type::Directional);
}