#pragma once
#include "GrowthEngine.h"
#include "BehaviorTreeSetting/BehaviorTreeSetting.h"
#include "BehaviorTreeProjectManager/BehaviorTreeProjectManager.h"

// ビヘイビアツリーエディタ
class BehaviorTreeEditor
{
public:

	/// @brief セレクタノードを追加する
	void AddPersistentSelectorNode();

	/// @brief シーケンスノードを追加する
	void AddPersistentSequenceNode();

	/// @brief セレクタノードを追加する
	void AddRestartingSelectorNode();

	/// @brief シーケンスノードを追加する
	void AddRestartingSequenceNode();

	/// @brief 条件ノードを追加する
	void AddConditionNode();



	/// @brief ノードテーブルを描画する
	void DrawNodeTable();


private:

	/// @brief 現在編集中のツリーのファイル名
	std::string currentFileName_;

	/// @brief プロジェクトマネージャー
	BehaviorTreeProjectManager projectManager_;

	/// @brief ツリー構造の保存と読み込みを担当するクラス
	BehaviorTreeSetting saver_{ "BehaviorTree" };


private:

	// ノードリスト
	std::vector<EditorNode> nodes_;

	// リンクリスト
	std::vector<EditorLink> links_;

	// ノードとリンクのID生成用カウンタ
	int currentId_ = 1;

	// 次に割り当てるID
	int nextId_ = 1;


private:

	// 次のIDを取得してインクリメントする
	int GetNextId() { return currentId_++; }

	/// @brief エディタを初期状態にリセットする
	void ClearEditor();

	/// @brief 現在のツリー構造をファイルに保存する
	void SaveCurrentTree();

	/// @brief ファイルからツリー構造を読み込む
	/// @param fileName 
	void LoadTree(const std::string& fileName);
};

