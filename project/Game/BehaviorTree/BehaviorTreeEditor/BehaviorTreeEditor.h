#pragma once
#include "GrowthEngine.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeSetting/BehaviorTreeSetting.h"
#include "BehaviorTree/BehaviorTreeProjectManager/BehaviorTreeProjectManager.h"

class Character;

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

	/// @brief アクションノードを追加する
	void AddActionNode();



	/// @brief エディタ上のノードとリンクからビヘイビアツリーを生成する
	/// @param fileName 
	/// @return 
	std::unique_ptr<BehaviorTree> CreateTree(const std::string& fileName, Character* character);

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

	/// @brief 選択されているノードを削除する
	void DeleteSelectedNodes();

	/// @brief ファイルからツリー構造を読み込む
	/// @param fileName 
	void LoadTree(const std::string& fileName);

	/// @brief ノードをウィンドウの中心に配置する
	/// @param node 
	void SetNodeWindowCenter(EditorNode node);


private:

	/// @brief コピーしたノードとリンクの情報を保持する変数
	void HandleCopy();

	/// @brief コピーしたノードとリンクの情報をクリップボードに保存する
	void HandlePaste();


	/// @brief コピーしたノードの情報を保持する変数
	std::vector<EditorNode> clipboardNodes_;

	/// @brief コピーしたリンクの情報を保持する変数
	std::vector<EditorLink> clipboardLinks_;
};

