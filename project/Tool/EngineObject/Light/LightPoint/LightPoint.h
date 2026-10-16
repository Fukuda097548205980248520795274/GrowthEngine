#pragma once
#include "../BaseLight.h"

/// @brief エンジン
class GrowthEngine;

/// @brief ポイントライト
class LightPoint : public Engine::BaseLight
{
public:

	/// @brief コンストラクタ
	/// @param name 
	LightPoint(const std::string& name);


	/// @brief パラメータ
	Engine::Light::PointLightParam* param_{};
};

