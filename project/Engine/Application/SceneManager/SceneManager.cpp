#include "SceneManager.h"
#include "GameScene/GameScene.h"
#include "TitleScene/TitleScene.h"
#include "GrowthEngine.h"

/// @brief コンストラクタ
/// @param sceneName 
SceneManager::SceneManager(const std::string& sceneName) :sceneName_(sceneName) 
{
	// エンジンのインスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// 最初は遷移する
	isTransition_ = true; 
}

/// @brief 更新処理
void SceneManager::Update()
{
	// 遷移するとき
	if (isTransition_)
	{
		// 使用中のプレハブインスタンスを全て消す
		engine_->DestroyAllPrefabInstance();

		// ゲームシーン
		if (sceneName_ == "Game")
		{
			scene_ = std::make_unique<GameScene>(this);
			scene_->Initialize();
			isTransition_ = false;
		}

		// タイトルシーン
		if (sceneName_ == "Title")
		{
			scene_ = std::make_unique<TitleScene>(this);
			scene_->Initialize();
			isTransition_ = false;
		}

		// まだ遷移中は処理しない
		assert(!isTransition_);
	}

	// シーンの更新
	scene_->Update();
}

/// @brief 描画処理
void SceneManager::Draw()
{
	// シーンの描画
	scene_->Draw();
}