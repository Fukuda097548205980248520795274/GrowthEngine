#include "DirectionalLightData.h"

/// @brief コンストラクタ
/// @param name 
/// @param handle 
Engine::DirectionalLightData::DirectionalLightData(const std::string& name, LightHandle handle) 
	: BaseLightData(name, handle) 
{
	// パラメータ生成
	param_ = std::make_unique<DirectionalLightParam>();
	param_->direction = Vector3(0.0f, -1.0f, 0.0f);
	param_->intensity = 1.0f;
	param_->color = Vector3(1.0f, 1.0f, 1.0f);
}