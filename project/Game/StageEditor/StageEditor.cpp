#include "StageEditor.h"
#include <numbers>
#include <json.hpp>
#include <set>
#include "Scene/GameScene/GameScene.h"

/// @brief 初期化
void StageEditor::Initialize()
{
	// ファイルマネージャの初期化
	fileManager_ = std::make_unique<StageFileManager>(kStageDataDir);

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
#ifdef DEVELOPMENT

	// 実行中に消えたエンティティを配置リストから削除する
	if (isPlaying_)
	{
		for (int i = static_cast<int>(placementList_.size()) - 1; i >= 0; --i)
		{
			auto& data = placementList_[i];

			// std::variantを使っているため、std::visitを使って型ごとに処理する
			bool shouldDelete = std::visit([](auto&& ptr) -> bool 
				{
					using T = std::decay_t<decltype(ptr)>;
					if constexpr (std::is_same_v<T, std::monostate>)
					{
						return false;
					}
					else
					{
						return ptr && ptr->IsFinished();
					}
				}, 
				data.instancePtr);

			// 配置リストから削除する
			if (shouldDelete)
			{
				placementList_.erase(placementList_.begin() + i);

				// 選択中のインデックスを更新する
				int currentSelected = editorUI_->GetSelectedIndex();
				if (currentSelected == i)
				{
					editorUI_->SetSelectedIndex(-1);
				}
				else if (currentSelected > i)
				{
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
	if (fileManager_->LoadFromFile(fileName + ".json", placementList_, spawner_.get(), scene_->GetNavMesh()))
	{
		currentFileName_ = fileName + ".json";

		editorUI_->Play(isPlaying_);

		// ステージをロードしたことをシーンに通知する
		scene_->OnStageLoaded(fileName);
	}
}

/// @brief 描画処理（デバッグ用）
void StageEditor::DrawUI()
{
#ifdef DEVELOPMENT

	navMeshController_->DrawDebug();

	editorUI_->DrawAssetWindow(placementList_, currentFileName_, isPlaying_, scene_->GetNavMesh(), isDirty_);
	editorUI_->DrawUI(placementList_, currentFileName_, isPlaying_, scene_->GetNavMesh(), navMeshController_.get(),
		isDirty_, navMeshController_->CanExtrudeSelectedEdge(), navMeshController_->CanBridgeSelectedEdges());

#endif
}