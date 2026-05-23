#include "Prefab3DBaseData.h"

/// @brief コンストラクタ
/// @param name 
/// @param numInstance 
/// @param hPrefab 
Engine::Prefab3DBaseData::Prefab3DBaseData(const std::string& name, uint32_t numInstance, Prefab3DHandle hPrefab3D, BasePSOModel* pso, Prefab3DParameter* parameter)
	: name_(name), numInstance_(numInstance), hPrefab3D_(hPrefab3D), pso_(pso), parameter_(parameter) 
{
	// 読み込まれた
	isLoad_ = true;
}

/// @brief シーン前のリセット
void Engine::Prefab3DBaseData::PerSceneReset()
{
	// デバッグ用はリセットしない
	if (isDebug_)return;

	// 読み込みを初期化する
	isLoad_ = false;

	// 全てのインスタンスを削除する
	DestroyAllInstance();
}