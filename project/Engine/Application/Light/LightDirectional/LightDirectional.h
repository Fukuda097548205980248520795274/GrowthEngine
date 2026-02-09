#pragma once
#include "Data/LightData/LightData.h"
#include "Handle/Handle.h"
#include <string>

class LightDirectional
{

public:

	/// @brief コンストラクタ
	/// @param name 
	LightDirectional(const std::string& name);

	/// @brief 設置
	void Set();


	/// @brief パラメータ
	Engine::DirectionalLightParam* param_{};


private:

	/// @brief 名前
	std::string name_{};

	/// @brief ハンドル
	LightHandle handle_ = 0;
};

