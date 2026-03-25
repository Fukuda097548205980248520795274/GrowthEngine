#include "Render3DBaseData.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
/// @param hRender3D 
/// @param parameter 
Engine::Render3DBaseData::Render3DBaseData(const std::string& name, Render3DHandle hRender3D, Render3DParameter* parameter)
	: name_(name), hRender3D_(hRender3D), parameter_(parameter) 
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 読み込み
	isLoad_ = true;

	// 3D
	guizmoData_.dimension = DebugData::GuizmoDimension::Perspective;
}

/// @brief シーン前のリセット
void Engine::Render3DBaseData::PerSceneReset()
{
	// 読み込みをリセットする
	isLoad_ = false;
}