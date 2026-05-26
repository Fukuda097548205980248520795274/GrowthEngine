#pragma once
#include <vector>
#include "BehaviorTree/BehaviorTreeProjectManager/BehaviorTreeProjectManager.h"

class BehaviorTreeEditor;

class BehaviorTreeEditorHistory
{
public:

	/// @brief エディタのスナップショット
	struct EditorSnapshot
	{
		std::vector<EditorNode> nodes; // ノードのリスト
		std::vector<EditorLink> links; // リンクのリスト
		int currentId; // 現在のIDカウンタ
	};

	/// @brief 現在の状態を履歴に保存する
	/// @param nodes 
	/// @param links 
	/// @param currentId 
	void SaveHistory(const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links, int currentId);

	/// @brief Undo（元に戻す）を実行する
	void Undo(BehaviorTreeEditor& editor);

	/// @brief Redo（やり直す）を実行する
	void Redo(BehaviorTreeEditor& editor);

	/// @brief 履歴をクリアする
	void Clear();


private:

	// Undoの履歴
	std::vector<EditorSnapshot> undoHistory_;

	// Redoの履歴
	std::vector<EditorSnapshot> redoHistory_;
};

