#pragma once
#include "ComboTreeData/ComboTreeData.h"
#include "ComboTreeProjectManager/ComboTreeProjectManager.h"
#include "ComboTreeEditorClipboard/ComboTreeEditorClipboard.h"
#include "ComboTreeEditorHistory/ComboTreeEditorHistory.h"

class ComboTreeEditor
{
	// privateメンバにアクセスできるようにするためのフレンドクラス宣言
    friend class ComboTreeEditorClipboard;
	friend class ComboTreeEditorHistory;

public:

	/// @brief コンストラクタ
	ComboTreeEditor() = default;

	/// @brief ノードを追加する
    void AddComboAttackNode();

    /// @brief ファイルにコンボツリーを保存する
    /// @param filePath 
    void SaveToFile(const std::string& filePath);

    /// @brief ファイルからコンボツリーを読み込む
    /// @param filePath 
    void LoadFromFile(const std::string& filePath);

    /// @brief UI描画処理
    void DrawUI();

private:

	/// @brief プロジェクトパネルを描画する
	void DrawProjectPanel();

    /// @brief ノードエディタを描画する
    void DrawNodeEditor();

    /// @brief プロパティパネルを描画する
    void DrawPropertyPanel();


private:

    /// @brief リンクを削除する
    void DeleteLink();

    /// @brief 選択されているノードを削除する
    void DeleteSelectedNodes();

	/// @brief ユニークなIDを発行する関数
    /// @return 
    int GetNextId() { return currentId_++; }

	/// @brief 変更があったことを通知する関数
    void HandleChange();

	/// @brief 指定されたIDのノードを取得する関数
    /// @param id 
    /// @return 
    ComboEditorNode* GetNodeById(int id);


private:

	/// @brief プロジェクトマネージャー
	ComboTreeProjectManager projectManager_;

	// @brief コピー用のクリップボード管理クラス
    ComboTreeEditorClipboard clipboard_;

	/// @brief 履歴管理クラス
	ComboTreeEditorHistory history_;

	// ノード情報を保持する配列
    std::vector<ComboEditorNode> nodes_;

	// リンク情報を保持する配列
    std::vector<ComboEditorLink> links_;

	// ユニークなIDを発行するためのカウンタ
    int currentId_ = 1;

    // 現在のファイル名を保持する変数
    std::string currentFileName_{};

    // ノードをドラッグ中かどうかのフラグ
	bool isDraggingNode_ = false; 
};

