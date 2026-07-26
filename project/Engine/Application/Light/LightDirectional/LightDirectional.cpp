#include "LightDirectional.h"
#include "GrowthEngine.h"
#include "Store/LightStore/LightData/DirectionalLightData/DirectionalLightData.h"

/// @brief コンストラクタ
/// @param name 
LightDirectional::LightDirectional(const std::string& name) : BaseLight(name)
{
	// 種類を設定する
	type_ = Engine::Light::Type::Directional;

	// ライトを読み込む
	hLight_ = engine_->LoadLight(name, Engine::Light::Type::Directional);

	// パラメータを取得する
	param_ = engine_->GetLightParam<Engine::Light::DirectionalLightParam>(hLight_);
}