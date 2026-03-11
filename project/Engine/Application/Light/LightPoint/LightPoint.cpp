#include "LightPoint.h"
#include "GrowthEngine.h"
#include "Store/LightStore/LightData/PointLightData/PointLightData.h"

/// @brief コンストラクタ
	/// @param name 
LightPoint::LightPoint(const std::string& name) : BaseLight(name)
{
	// ライトを読み込む
	hLight_ = engine_->LoadLight(name, Engine::Light::Type::Point);

	// パラメータを取得する
	param_ = engine_->GetLightParam<Engine::Light::PointLightParam>(hLight_);
}