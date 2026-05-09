#pragma once
#include "Math/Vector/Vector2/Vector2.h"
#include <vector>
#include <string>

// ノードの種類
enum class EditorNodeType
{
	PersistentSelector,
	PersistentSequence,
	RestartingSelector,
	RestartingSequence,
	Condition,
};

// エディタ上のノードを表す構造体
struct EditorNode
{
	int id; // ノードのID
	EditorNodeType type; // ノードの種類

	int inputPinId; // 入力ピンのID（親とつなげる）
	int outputPinId; // 出力ピンのID（子とつなげる）

	Vector2 pos; // ノードの位置

	std::string conditionName; // 条件ノードの場合の条件名
};

// エディタ上のリンクを表す構造体
struct EditorLink
{
	int id; // リンクのID
	int startPinId; // リンクの開始ピンのID（親）
	int endPinId; // リンクの終了ピンのID（子）
};

class BehaviorTreeProjectManager
{
public:

	/// @brief コンストラクタ
    BehaviorTreeProjectManager();

	/// @brief 保存されているツリーのファイル名一覧を返す
    /// @return 
    std::vector<std::string> GetFileList();

	/// @brief プロジェクトファイルを削除する
	/// @param fileName 
	/// @return 
	bool DeleteProjectFile(const std::string& fileName);

	/// @brief プロジェクトファイルをコピーする
	/// @param sourceFileName 
	/// @param destFileName 
	/// @return 
	bool CopyProjectFile(const std::string& sourceFileName, const std::string& destFileName);

private:

	// ツリー構造の保存先ディレクトリパス
    std::string directoryPath_;
};

