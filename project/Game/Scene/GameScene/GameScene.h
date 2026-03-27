#pragma once
#include "GrowthEngine.h"
#include "Stage/Stage.h"
#include "StageEditor/StageEditor.h"
#include "StageSerializer/StageSerializer.h"

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


	// 太陽
	std::unique_ptr<LightDirectional> sun_ = nullptr;

	PrefabInstanceSprite* a1_ = nullptr;
	PrefabInstanceSprite* a2_ = nullptr;
};

