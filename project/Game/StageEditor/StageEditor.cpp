#include "StageEditor.h"
#include <numbers>
#include <json.hpp>
#include <set>
#include "Scene/GameScene/GameScene.h"

// ステージデータの保存先ディレクトリ
constexpr const char* STAGE_DATA_DIR = "./Assets/Parameter/StageData/";

/// @brief 初期化
void StageEditor::Initialize()
{
	// ファイルマネージャの初期化
	fileManager_ = std::make_unique<StageFileManager>(STAGE_DATA_DIR);

	// 履歴管理の初期化
	history_ = std::make_unique<StageEditorHistory>(scene_->GetNavMesh());

	// スペナーの初期化
	spawner_ = std::make_unique<StageSpawner>(scene_);
	spawner_->Initialize();

	// エディタUIの初期化
	editorUI_ = std::make_unique<StageEditorUI>(fileManager_.get(), spawner_.get(), history_.get(), scene_);
	editorUI_->Initialize();

	// ナビメッシュ編集コントローラーの初期化
	navMeshController_ = std::make_unique<StageEditorNavMeshController>(scene_->GetNavMesh(), history_.get());
}

/// @brief 更新処理
/// @param dt 
void StageEditor::Update(float dt)
{
#ifdef _DEVELOPMENT

	// 実行中に消えたエンティティを配置リストから削除する
	if (isPlaying_)
	{
		// 逆順にループして、消えたエンティティを削除する
		for (int i = static_cast<int>(placementList_.size()) - 1; i >= 0; --i)
		{
			auto& data = placementList_[i];
			bool shouldDelete = false;

			if (data.category == EditCategory::Character || data.category == EditCategory::Weapon)
			{
				Entity* entity = static_cast<Entity*>(data.instancePtr);
				if (entity && entity->IsFinished()) shouldDelete = true;
			}
			else if (data.category == EditCategory::Object)
			{
				StageObject* obj = static_cast<StageObject*>(data.instancePtr);
				if (obj && obj->IsFinished()) shouldDelete = true;
			}

			if (shouldDelete)
			{
				// 配置リストから要素を削除
				placementList_.erase(placementList_.begin() + i);

				// UI側の選択状態を安全に調整する
				int currentSelected = editorUI_->GetSelectedIndex();
				if (currentSelected == i)
				{
					// 選択していたオブジェクトそのものが消滅した場合、選択を解除する
					editorUI_->SetSelectedIndex(-1);
				}
				else if (currentSelected > i)
				{
					// 選択していたオブジェクトのインデックスが削除された要素より後ろにある場合、インデックスを1つ前にずらす
					editorUI_->SetSelectedIndex(currentSelected - 1);
				}
			}
		}
	}

	// UIの更新
	editorUI_->Update();

	// 現在のモードに応じた処理
	if (editorUI_->GetCurrentMode() == StageEditorUI::EditorMode::NavMeshEdit)
	{
		navMeshController_->Update(placementList_, isDirty_);
	}

#endif
}

/// @brief ステージを読み込む
/// @param fileName 
void StageEditor::LoadStage(const std::string& fileName)
{
	// ファイルを読み込む
	fileManager_->LoadFromFile(fileName, placementList_, spawner_.get(), scene_->GetNavMesh());
	editorUI_->Play(isPlaying_);
}

/// @brief 描画処理（デバッグ用）
void StageEditor::DrawUI()
{
#ifdef _DEVELOPMENT

	navMeshController_->DrawDebug();

	editorUI_->DrawAssetWindow(placementList_, currentFileName_, isPlaying_, scene_->GetNavMesh(), isDirty_);
	editorUI_->DrawUI(placementList_, currentFileName_, isPlaying_, scene_->GetNavMesh(), navMeshController_.get(),
		isDirty_, navMeshController_->CanExtrudeSelectedEdge(), navMeshController_->CanBridgeSelectedEdges());

#endif
}