#include "Scene.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param sceneManager 
Scene::Scene(SceneManager* sceneManager) : sceneManager_(sceneManager)
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();
}

/// @brief シーン遷移する
/// @param sceneName 
void Scene::Transition(const std::string& sceneName)
{
	sceneManager_->Transition(sceneName);
}