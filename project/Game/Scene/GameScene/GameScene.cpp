#include "GameScene.h"

/// @brief 初期化
void GameScene::Initialize()
{
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
	// ステージ描画
	stage_->Draw();
}