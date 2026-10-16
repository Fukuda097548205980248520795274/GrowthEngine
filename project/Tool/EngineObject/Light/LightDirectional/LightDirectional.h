#pragma once
#include "../BaseLight.h"

/// @brief エンジン
class GrowthEngine;

/// @brief 平行光源
class LightDirectional : public Engine::BaseLight
{
public:

	/// @brief コンストラクタ
	/// @param name 
	LightDirectional(const std::string& name);


	/// @brief パラメータ
	Engine::Light::DirectionalLightParam* param_{};
};

