#include "StageEditorHistory.h"
#include "../StageSpawner/StageSpawner.h"

/// @brief 履歴に新しいスナップショットを保存する
/// @param placementList 
void StageEditorHistory::SaveHistory(const std::vector<PlacementData>& placementList)
{
	EditorSnapshot snapshot;
	snapshot.placementList = placementList;

	// ナビメッシュのスナップショットも保存する
	if (navMesh_) snapshot.navPolygonList = navMesh_->GetPolygons();

	// 新しいスナップショットをUndo履歴に追加し、Redo履歴はクリアする
	undoHistory_.push_back(snapshot);
	redoHistory_.clear();

	// 履歴の上限を50に設定（必要に応じて調整可能）
	if (undoHistory_.size() > 50)
	{
		undoHistory_.erase(undoHistory_.begin());
	}
}

/// @brief 履歴に新しいスナップショットを保存する（ナビメッシュ用）
void StageEditorHistory::SaveHistory()
{
	// 過去の最新の配置データを取得してスナップショットに保存する
	EditorSnapshot snapshot;
	snapshot.placementList = undoHistory_.empty() ? std::vector<PlacementData>() : undoHistory_.back().placementList;

	// ナビメッシュのスナップショットを保存
	if (navMesh_) snapshot.navPolygonList = navMesh_->GetPolygons();

	// 新しいスナップショットをUndo履歴に追加し、Redo履歴はクリアする
	undoHistory_.push_back(snapshot);
	redoHistory_.clear();

	// 履歴の上限を50に設定（必要に応じて調整可能）
	if (undoHistory_.size() > 50)
	{
		undoHistory_.erase(undoHistory_.begin());
	}
}

/// @brief Undo（元に戻す）を実行する
/// @param currentList 
/// @param spawner 
/// @param navMesh 
void StageEditorHistory::Undo(std::vector<PlacementData>& currentList, StageSpawner* spawner)
{
	// Undo履歴が空の場合は何もしない
	if (undoHistory_.empty()) return;

	// 現在の状態をRedo履歴に保存しておく
	EditorSnapshot currentSnapshot;
	currentSnapshot.placementList = currentList;
	if (navMesh_)currentSnapshot.navPolygonList = navMesh_->GetPolygons();
	redoHistory_.push_back(currentSnapshot);

	// 現在のシーン上に配置されている実体をすべて削除する
	for (auto& data : currentList)
	{
		spawner->DeleteActualEntity(data);
	}

	// Undo履歴の最後尾から状態を復元
	EditorSnapshot snapshot = undoHistory_.back();
	undoHistory_.pop_back();

	currentList = snapshot.placementList;

	// ナビメッシュも復元する
	if (navMesh_) 
	{
		navMesh_->Clear();

		// 保存されていたポリゴンを再追加する
		for (const auto& poly : snapshot.navPolygonList)
			navMesh_->AddPolygon(poly);
	}

	// 復元したデータをもとに、実体をシーンに再生成する
	for (auto& data : currentList)
	{
		data.instancePtr = nullptr;

		PlacementData weaponData = {};

		spawner->SpawnActualEntity(data, weaponData);
	}
}

/// @brief Redo（やり直す）を実行する
/// @param currentList 
/// @param spawner 
/// @param navMesh 
void StageEditorHistory::Redo(std::vector<PlacementData>& currentList, StageSpawner* spawner)
{
	// Redo履歴が空の場合は何もしない
	if (redoHistory_.empty()) return;

	// 現在の状態をUndo履歴に保存しておく
	EditorSnapshot currentSnapshot;
	currentSnapshot.placementList = currentList;
	if (navMesh_)currentSnapshot.navPolygonList = navMesh_->GetPolygons();
	undoHistory_.push_back(currentSnapshot);

	// 現在のシーン上に配置されている実体をすべて削除する
	for (auto& data : currentList)
	{
		spawner->DeleteActualEntity(data);
	}

	// Redo履歴の最後尾から状態を復元
	EditorSnapshot snapshot = redoHistory_.back();
	redoHistory_.pop_back();

	currentList = snapshot.placementList;

	// ナビメッシュも復元する
	if (navMesh_) 
	{
		navMesh_->Clear();

		// 保存されていたポリゴンを再追加する
		for (const auto& poly : snapshot.navPolygonList)
		{
			navMesh_->AddPolygon(poly);
		}
	}

	// 復元したデータをもとに、実体をシーンに再生成する
	for (auto& data : currentList)
	{
		data.instancePtr = nullptr;

		PlacementData weaponData = {};

		spawner->SpawnActualEntity(data, weaponData);
	}
}

/// @brief 履歴をクリアする
void StageEditorHistory::Clear()
{
	undoHistory_.clear();
	redoHistory_.clear();
}