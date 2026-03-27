#include "Prefab2DBaseData.h"

/// @brief コンストラクタ
/// @param hPrefab2D 
/// @param numInstance 
/// @param name 
/// @param parameter 
Engine::Prefab2DBaseData::Prefab2DBaseData(Prefab2DHandle hPrefab2D, uint32_t numInstance, const std::string& name, Prefab2DParameter* parameter)
	: hPrefab2D_(hPrefab2D), numInstance_(numInstance), name_(name), parameter_(parameter)
{
	// 読み込まれたこととする
	isLoad_ = true;
}

/// @brief シーン前のリセット
void Engine::Prefab2DBaseData::PerSceneReset()
{
	// 読み込み初期化
	isLoad_ = false;

	// 全てのインスタンスを削除する
	DestroyAllInstance();
}