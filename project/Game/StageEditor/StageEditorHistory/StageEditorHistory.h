#pragma once
#include <vector>
#include "../StageData/StageData.h"
#include "NavMesh/NavMesh.h"

class StageSpawner;

class StageEditorHistory
{
public:

	/// @brief 履歴に新しいスナップショットを追加
	struct EditorSnapshot
	{
		// 配置データのスナップショット
		std::vector<PlacementData> placementList;

		// ナビメッシュのスナップショット
        std::vector<NavPolygon> navPolygonList;
	};


public:

	/// @brief コンストラクタ
    /// @param navMesh 
    StageEditorHistory(NavMesh* navMesh = nullptr) : navMesh_(navMesh) {}

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


private:

	// ナビメッシュへのポインタ
	NavMesh* navMesh_ = nullptr;
};

