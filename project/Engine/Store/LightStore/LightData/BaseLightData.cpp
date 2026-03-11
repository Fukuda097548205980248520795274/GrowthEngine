#include "BaseLightData.h"
#include "Parameter/LightParameter/LightParameter.h"

/// @brief 初期化
/// @param parameter 
void Engine::BaseLightData::Initialize(LightParameter* parameter)
{
	// nullptrチェック
	assert(parameter);

	// 引数を受け取る
	parameter_ = parameter;
}