#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
	sun_ = std::make_unique<LightDirectional>("Sun");

	// ステージ読み込み
	serializer_ = std::make_unique<StageSerializer>();

	// ステージデータ
	stageData_ = std::make_unique<StageData>();
	serializer_->LoadCSV(*stageData_, "stage");

	// ステージ
	stage_ = std::make_unique<Stage>();
	stage_->Initialize(stageData_.get());

	// エディター
	editor_ = std::make_unique<StageEditor>(*stageData_);

	engine_->LoadFont("String", "C:/Windows/Fonts/arial.ttf", 32);

	sprite_ = std::make_unique<Sprite>(engine_->LoadTexture("./Assets/Textures/uvChecker.png"), "sprite_test");
}

/// @brief 更新処理
void GameScene::Update()
{
	// エディタの更新
	editor_->UpdateEditorUI(serializer_.get());

	// ステージ更新
	stage_->Update();
}

/// @brief 描画処理
void GameScene::Draw()
{
	engine_->DrawDebugCube(Vector3(-5.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 1.0f, 1.0f), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	engine_->DrawDebugCube(Vector3(-5.0f, 3.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.5f, 1.0f), Vector4(0.0f, 0.0f, 1.0f, 1.0f));

	// ステージ描画
	stage_->Draw();

	sprite_->Draw();
}