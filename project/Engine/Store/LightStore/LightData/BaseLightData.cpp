#include "BaseLightData.h"
#include "Parameter/LightParameter/LightParameter.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param handle 
Engine::BaseLightData::BaseLightData(const std::string& name, LightHandle handle) : name_(name), handle_(handle) 
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 読み込む
	isLoad_ = true; 
}