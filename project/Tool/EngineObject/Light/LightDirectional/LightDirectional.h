#pragma once
#include "../BaseLight.h"

class GrowthEngine;

class LightDirectional : public Engine::BaseLight
{
public:

	/// @brief コンストラクタ
	/// @param name 
	LightDirectional(const std::string& name);


	/// @brief パラメータ
	Engine::Light::DirectionalLightParam* param_{};
};

