#include "LightDirectional.h"
#include "GrowthEngine.h"
#include "Store/LightStore/LightData/DirectionalLightData/DirectionalLightData.h"

/// @brief コンストラクタ
/// @param name 
LightDirectional::LightDirectional(const std::string& name) 
	: name_(name)
{
	// エンジンのインスタンスを取得する
	const GrowthEngine* engine = GrowthEngine::GetInstance();

	// ライトを読み込む
	handle_ = engine->LoadLight(name, "Directional");

	auto* param = static_cast<Engine::DirectionalLightParam*>(engine->GetLightParam(handle_));
	param_.directional = param->direction.get();
	param_.intensity = param->intensity.get();
	param_.color = param->color.get();
}

/// @brief 設置
void LightDirectional::Set()
{

}