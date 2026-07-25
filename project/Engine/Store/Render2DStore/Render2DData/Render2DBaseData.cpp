#include "Render2DBaseData.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param hSprite 
Engine::Render2DBaseData::Render2DBaseData(Render2DHandle hRender2D, std::string name) 
	: hRender2D_(hRender2D), name_(name)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 読み込む
	isLoad_ = true;
}

/// @brief シーン前のリセット
void Engine::Render2DBaseData::PerSceneReset()
{
	// 読み込みリセット
	isLoad_ = false;
}