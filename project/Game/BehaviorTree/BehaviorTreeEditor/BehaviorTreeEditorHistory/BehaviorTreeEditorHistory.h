#pragma once
#include <vector>
#include "../BehaviorTreeProjectManager/BehaviorTreeProjectManager.h"

class BehaviorTreeEditor;

class BehaviorTreeEditorHistory
{
public:

	/// @brief エディタのスナップショット
	struct EditorSnapshot
	{
		// ノードのリスト
		std::vector<EditorNode> nodes;

		// リンクのリスト
		std::vector<EditorLink> links;

		// 現在のIDカウンタ
		int currentId;

		// 選択されていたノードのID
		std::vector<int> selectedNodes;

		// 選択されていたリンクのID
		std::vector<int> selectedLinks;
	};

	/// @brief 現在の状態を履歴に保存する
	/// @param nodes 
	/// @param links 
	/// @param currentId 
	void SaveHistory(const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links, int currentId);

	/// @brief Undo（元に戻す）を実行する
	/// @param editor 
	void Undo(BehaviorTreeEditor& editor);

	/// @brief Redo（やり直す）を実行する
	/// @param editor 
	void Redo(BehaviorTreeEditor& editor);

	/// @brief 履歴をクリアする
	void Clear();

	/// @brief Undoが可能かどうかを返す
	/// @return 
	bool CanUndo() const { return !undoHistory_.empty(); }

	/// @brief Redoが可能かどうかを返す
	/// @return 
	bool CanRedo() const { return !redoHistory_.empty(); }


private:

	// Undoの履歴
	std::vector<EditorSnapshot> undoHistory_;

	// Redoの履歴
	std::vector<EditorSnapshot> redoHistory_;
};

