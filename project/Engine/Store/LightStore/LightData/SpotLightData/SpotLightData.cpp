#include "SpotLightData.h"

/// @brief コンストラクタ
/// @param name 
/// @param handle 
Engine::SpotLightData::SpotLightData(const std::string& name, LightHandle handle) : BaseLightData(name, handle)
{
	// パラメータの生成と初期化
	param_ = std::make_unique<Light::SpotLightParam>();
	param_->position = Vector3(0.0f, 0.0f, 0.0f);
	param_->color = Vector3(1.0f, 1.0f, 1.0f);
	param_->direction = Vector3(0.0f, 1.0f, 0.0f);
	param_->intensity = 12.0f;
	param_->distance = 5.0f;
	param_->decay = 4.0f;
	param_->cosAngle = 0.3f;
	param_->cosFalloffStart = 1.0f;
}