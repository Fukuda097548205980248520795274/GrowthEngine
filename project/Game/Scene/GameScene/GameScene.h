#pragma once
#include "GrowthEngine.h"
#include "Context/Context.h"

class GameScene : public Scene
{
public:

	/// @brief コンストラクタ
	/// @param sceneManager 
	GameScene(SceneManager* sceneManager) : Scene(sceneManager) {}

	/// @brief 初期化
	void Initialize() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;

private:

	// 全体で使用する
	std::unique_ptr<Context> context_ = nullptr;
};

