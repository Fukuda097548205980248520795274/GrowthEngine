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

	/// @brief ステージ読み込み
	std::unique_ptr<StageSerializer> serializer_ = nullptr;

	/// @brief ステージデータ
	std::unique_ptr<StageData> stageData_ = nullptr;

	/// @brief ステージ
	std::unique_ptr<Stage> stage_ = nullptr;

	/// @brief エディター
	std::unique_ptr<StageEditor> editor_ = nullptr;


	// 太陽
	std::unique_ptr<LightDirectional> sun_ = nullptr;
};

