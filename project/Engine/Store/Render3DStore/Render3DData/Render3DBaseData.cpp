#include "Render3DBaseData.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param hRender3D 
/// @param parameter 
Engine::Render3DBaseData::Render3DBaseData(const std::string& name, Render3DHandle hRender3D)
	: name_(name), hRender3D_(hRender3D)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 読み込み
	isLoad_ = true;
}

/// @brief シーン前のリセット
void Engine::Render3DBaseData::PerSceneReset()
{
	// 読み込みをリセットする
	isLoad_ = false;
}