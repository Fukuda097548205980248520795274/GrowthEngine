#pragma once
#include "../BaseLight.h"

/// @brief エンジン
class GrowthEngine;

/// @brief スポットライト
class LightSpot : public Engine::BaseLight
{
public:

	/// @brief コンストラクタ
	/// @param name 
	LightSpot(const std::string& name);


	/// @brief パラメータ
	Engine::Light::SpotLightParam* param_{};
};

