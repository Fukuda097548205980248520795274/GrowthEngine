#include "StageEditor.h"
#include <numbers>
#include <json.hpp>
#include "Scene/GameScene/GameScene.h"

/// @brief 初期化
void StageEditor::Initialize()
{
	// ファイルマネージャの初期化
	fileManager_ = std::make_unique<StageFileManager>("./Assets/Parameter/StageData/");

	// スペナーの初期化
	spawner_ = std::make_unique<StageSpawner>(scene_);
	spawner_->Initialize();

	// エディタUIの初期化
	editorUI_ = std::make_unique<StageEditorUI>(fileManager_.get(), spawner_.get());
	editorUI_->Initialize();
}

/// @brief 更新処理
/// @param dt 
void StageEditor::Update(float dt)
{

}

/// @brief 描画処理（デバッグ用）
void StageEditor::DrawUI()
{
	editorUI_->DrawAssetWindow(placementList_, currentFileName_);
	editorUI_->DrawUI(placementList_, currentFileName_);
	editorUI_->DrawObjectListWindow(placementList_);
}