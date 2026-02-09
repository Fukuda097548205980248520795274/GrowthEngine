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

	auto param = engine->GetLightParam<Engine::DirectionalLightParam>(handle_);
	param_ = param;
}

/// @brief 設置
void LightDirectional::Set()
{

}