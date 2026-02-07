#include "DirectionalLightData.h"

/// @brief コンストラクタ
/// @param name 
/// @param handle 
Engine::DirectionalLightData::DirectionalLightData(const std::string& name, LightHandle handle) 
	: BaseLightData(name, handle) 
{
	// パラメータ生成
	param_.direction = std::make_unique<Vector3>(0.0f, -1.0f, 0.0f);
	param_.intensity = std::make_unique<float>(1.0f);
	param_.color = std::make_unique<Vector4>(1.0f, 1.0f, 1.0f, 1.0f);
}