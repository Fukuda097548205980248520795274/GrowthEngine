#pragma once
#include "GrowthEngine.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTreeSetting/BehaviorTreeSetting.h"
#include "BehaviorTreeProjectManager/BehaviorTreeProjectManager.h"
#include "BehaviorTreeEditorClipboard/BehaviorTreeEditorClipboard.h"
#include "BehaviorTreeEditorHistory/BehaviorTreeEditorHistory.h"
#include <unordered_set>

class Character;

// ビヘイビアツリーエディタ
class BehaviorTreeEditor
{
public:

	/// @brief コンストラクタ
	BehaviorTreeEditor();

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

	/// @brief UIを描画する
	void DrawUI();


private:

	/// @brief ノードテーブルを描画する
	void DrawNodeTable();

	/// @brief プロパティウィンドウを描画する
	void DrawPropertyWindow();

	/// @brief　プロジェクトウィンドウを描画する
	void DrawProjectWindow();


private:

	/// @brief 現在編集中のツリーのファイル名
	std::string currentFileName_;

	/// @brief プロジェクトマネージャー
	BehaviorTreeProjectManager projectManager_;

	/// @brief ツリー構造の保存と読み込みを担当するクラス
	BehaviorTreeSetting saver_{ "BehaviorTree" };


	/// @brief 履歴管理クラス
	std::unique_ptr<BehaviorTreeEditorHistory> history_ = nullptr;

	/// @brief クリップボード管理クラス
	std::unique_ptr<BehaviorTreeEditorClipboard> clipboard_ = nullptr;

	// エディタのUI描画に使用するフラグ
	friend class BehaviorTreeEditorHistory;
	friend class BehaviorTreeEditorClipboard;


private:

	// ノードの展開状態を管理するマップ（ノードIDと展開状態のペア）
	std::unordered_map<int, bool> collapsedNodes_;

	// 前のフレームで展開されていたノードのIDを保持するセット
	std::unordered_set<int> prevHiddenNodes_;


private:

	/// @brief エンジンのインスタンス
	const GrowthEngine* engine_ = GrowthEngine::GetInstance();

	// ファイルアイコンのSRVのGPUハンドル
	D3D12_GPU_DESCRIPTOR_HANDLE btFileIcon_;


private:

	// ノードリスト
	std::vector<EditorNode> nodes_;

	// リンクリスト
	std::vector<EditorLink> links_;

	// ノードとリンクのID生成用カウンタ
	int currentId_ = 1;


	// ノードをウィンドウの中心に配置するための保留中のノードID
	int pendingCenterNodeId_ = -1;


private:

	// 未保存の変更があるかどうかのフラグ
	bool isDirty_ = false;

	// 次に読み込む、または新規作成するファイル名
	std::string pendingFileName_ = "";

	// 保留中の操作が「新規作成」かどうか
	bool isPendingNewTree_ = false;


private:

	// ズーム倍率
	float zoom_ = 1.0f;

	// 前フレームのズーム倍率
	float prevZoom_ = 1.0f;


private:

	// 次のIDを取得してインクリメントする
	int GetNextId() { return currentId_++; }

	/// @brief エディタを初期状態にリセットする
	void ClearEditor();

	/// @brief 現在のツリー構造をファイルに保存する
	void SaveCurrentTree();

	/// @brief 選択されているノードを削除する
	void DeleteSelectedNodes();

	/// @brief ノードを自動配置する
	void AutoArrangeNodes();

	/// @brief ファイルからツリー構造を読み込む
	/// @param fileName 
	void LoadTree(const std::string& fileName);

	/// @brief ノードをウィンドウの中心に配置する
	/// @param node 
	void SetNodeWindowCenter(const EditorNode& node);

	/// @brief ルートノードのIDを取得する関数
	/// @return 
	int FindRootNodeId() const;


private:

	/// @brief ノードエディタのキャンバスを描画する
	void DrawNodeEditorCanvas();

	/// @brief ノードの内容を描画する
	/// @param node 
	void DrawNodeContent(EditorNode& node);

	/// @brief 条件ノードの設定UIを描画する
	/// @param node 
	void DrawCondtionNodeSettings(EditorNode& node);

	/// @brief アクションノードの設定UIを描画する
	/// @param node 
	void DrawActionNodeSettings(EditorNode& node);
};

