#include "BaseRender3D.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param name 
Engine::BaseRender3D::BaseRender3D(const std::string& name) : name_(name)
{
	engine_ = GrowthEngine::GetInstance();
}

/// @brief 親を設定する
/// @param parent 
void Engine::BaseRender3D::SetParent(WorldTransform3D* parent)
{
	engine_->SetRender3DParent(hRender3D_, parent);
}