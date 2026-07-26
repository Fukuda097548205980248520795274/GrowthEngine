#pragma once
#include "../BaseLight.h"

class GrowthEngine;

class LightSpot : public Engine::BaseLight
{
public:

	/// @brief コンストラクタ
	/// @param name 
	LightSpot(const std::string& name);


	/// @brief パラメータ
	Engine::Light::SpotLightParam* param_{};
};

