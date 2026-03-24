#include "BaseLightData.h"
#include "Parameter/LightParameter/LightParameter.h"

/// @brief コンストラクタ
/// @param name 
/// @param handle 
Engine::BaseLightData::BaseLightData(const std::string& name, LightHandle handle) : name_(name), handle_(handle) 
{
	// 読み込む
	isLoad_ = true; 
}

/// @brief 初期化
/// @param parameter 
void Engine::BaseLightData::Initialize(LightParameter* parameter)
{
	// nullptrチェック
	assert(parameter);

	// 引数を受け取る
	parameter_ = parameter;
}