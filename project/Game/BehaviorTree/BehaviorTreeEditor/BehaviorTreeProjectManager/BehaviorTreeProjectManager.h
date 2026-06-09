#pragma once
#include "Math/Vector/Vector2/Vector2.h"
#include <vector>
#include <string>
#include "Action/Attack/ComboAttack/ComboAttack.h"
#include "Action/Attack/GrabAttack/GrabAttack.h"
#include "Action/Attack/GrabStrikeAttack/GrabStrikeAttack.h"
#include "Action/Avoid/Avoid.h"
#include "Action/Move/ApproachTargetMove/ApproachTargetMove.h"
#include "Action/Move/NavMeshMove/NavMeshMove.h"
#include "MotionManager/MotionManager.h"

// ノードの種類
enum class EditorNodeType
{
	PersistentSelector,
	PersistentSequence,
	RestartingSelector,
	RestartingSequence,
	Condition,
	Action,
};

enum class ConditionType
{
	None = 0,

	// ターゲットがいるかどうか
	HasTarget,

	// ターゲットがダウンしているかどうか
	IsTargetDown,

	/// @brief ターゲットがダウンしていないかどうか
	IsNotTargetDown,

	IsGrabbing, // 掴んでいるかどうか

	IsNotGrabbing, // 掴んでいないかどうか

	IsTargetInRange, // ターゲットが一定距離内にいるかどうか

	IsTargetOutOfRange, // ターゲットが一定距離外にいるかどうか

	isTargetAttacking, // ターゲットが攻撃しているかどうか
	isTargetNotAttacking, // ターゲットが攻撃していないかどうか

	isTargetInAttackSequence, // ターゲットが攻撃動作中かどうか
	isTargetNotInAttackSequence, // ターゲットが攻撃動作中でないかどうか
};

/// @brief 条件ノードパラメータ
struct CondtionNodeParam
{
	float distanceToTarget = 0.0f; // ターゲットとの距離
};

// エディタ上のノードを表す構造体
struct EditorNode
{
	int id; // ノードのID
	EditorNodeType type; // ノードの種類

	int inputPinId; // 入力ピンのID（親とつなげる）
	int outputPinId; // 出力ピンのID（子とつなげる）

	Vector2 pos; // ノードの位置

	std::string actionName{}; // アクションノードの場合のアクション名

	// 条件ノードの場合の条件の種類
	ConditionType conditionType = ConditionType::None;

	MotionType motionType = MotionType::Stand; // 条件ノードでモーションを条件にする場合のモーションの種類
	std::string motionName{}; 

	MotionType targetMotionType = MotionType::Stand; // 条件ノードでターゲットのモーションを条件にする場合のモーションの種類
	std::string targetMotionName{};

	// 初期化用データ（アクションノードの種類に応じて使用）
	CombAttackInitData comboAttackInitData;
	GrabAttackInitData grabAttackInitData;
	GrabStrikeAttackInitData grabStrikeAttackInitData;
	AvoidInitData avoidInitData;
	ApproachTargetMoveInitData approachTargetMoveInitData;
	NavMeshMoveInitData navMeshMoveInitData;

	// 条件ノードのパラメータ
	CondtionNodeParam conditionParam;

	// ノードが展開されているかどうかを示すフラグ（条件ノードの設定UIを表示するかどうかの判定に使用）
	bool isCollapsed = false;

	// ノードの位置を次の描画フレームでImNodesに反映する必要があるかどうかを示すフラグ
	bool needSetPos = true;
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

