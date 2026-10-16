#pragma once
#include "GrowthEngine.h"

/// @brief ライトエディタ用データ
struct LightElementData
{
	// Light要素の名前（ヒエラルキー表示用）
	std::string name;

	/// @brief 保存・復元用のライトの種類
	Engine::Light::Type lightType = Engine::Light::Type::None;

	/// @brief ライト
	std::unique_ptr<Engine::BaseLight> light;
};