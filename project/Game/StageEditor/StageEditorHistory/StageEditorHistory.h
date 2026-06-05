#pragma once
#include <vector>
#include "../StageData/StageData.h"

class StageSpawner;

class StageEditorHistory
{
public:

	/// @brief 履歴に新しいスナップショットを追加
	struct EditorSnapshot
	{
		std::vector<PlacementData> placementList;
	};


public:

	/// @brief 履歴に新しいスナップショットを保存する
    /// @param placementList 
    void SaveHistory(const std::vector<PlacementData>& placementList);

	/// @brief Undo（元に戻す）を実行する
    /// @param currentList 
    /// @param spawner 
    void Undo(std::vector<PlacementData>& currentList, StageSpawner* spawner);

	/// @brief Redo（やり直す）を実行する
    /// @param currentList 
    /// @param spawner 
    void Redo(std::vector<PlacementData>& currentList, StageSpawner* spawner);

    /// @brief 履歴をクリアする
    void Clear();

private:

	/// @brief Undo用のスナップショットのリスト
    std::vector<EditorSnapshot> undoHistory_;

	/// @brief Redo用のスナップショットのリスト
    std::vector<EditorSnapshot> redoHistory_;
};

