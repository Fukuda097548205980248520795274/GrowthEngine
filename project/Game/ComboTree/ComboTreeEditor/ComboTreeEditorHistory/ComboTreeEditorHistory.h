#pragma once
#include "../ComboTreeData/ComboTreeData.h"

class ComboTreeEditor;

class ComboTreeEditorHistory
{
public:

	/// @brief エディタのスナップショット
	struct EditorSnapshot
	{
		// ノードのリスト
		std::vector<ComboEditorNode> nodes;

		// リンクのリスト
		std::vector<ComboEditorLink> links;

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
	void SaveHistory(const std::vector<ComboEditorNode>& nodes, const std::vector<ComboEditorLink>& links, int currentId);

	/// @brief Undo（元に戻す）を実行する
	/// @param editor 
	void Undo(ComboTreeEditor& editor);

	/// @brief Redo（やり直す）を実行する
	/// @param editor 
	void Redo(ComboTreeEditor& editor);

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

