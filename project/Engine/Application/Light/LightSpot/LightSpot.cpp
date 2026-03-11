#include "LightSpot.h"
#include "GrowthEngine.h"
#include "Store/LightStore/LightData/SpotLightData/SpotLightData.h"

/// @brief コンストラクタ
	/// @param name 
LightSpot::LightSpot(const std::string& name) : BaseLight(name)
{
	// ライトを読み込む
	hLight_ = engine_->LoadLight(name, Engine::Light::Type::Spot);

	// パラメータを取得する
	param_ = engine_->GetLightParam<Engine::Light::SpotLightParam>(hLight_);
}