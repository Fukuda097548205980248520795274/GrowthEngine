#include "PointLightData.h"

/// @brief コンストラクタ
/// @param name 
/// @param handle 
Engine::PointLightData::PointLightData(const std::string& name, LightHandle handle) : BaseLightData(name,handle)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Light::PointLightParam>();
	param_->position = Vector3(0.0f, 0.0f, 0.0f);
	param_->color = Vector3(1.0f, 1.0f, 1.0f);
	param_->intensity = 12.0f;
	param_->radius = 5.0f;
	param_->decay = 4.0f;
}