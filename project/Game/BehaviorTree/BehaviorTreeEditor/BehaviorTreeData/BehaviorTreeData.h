#pragma once
#include "GrowthEngine.h"

#include "Action/Attack/ComboAttack/ComboAttack.h"
#include "Action/Attack/GrabAttack/GrabAttack.h"
#include "Action/Attack/GrabStrikeAttack/GrabStrikeAttack.h"
#include "Action/Avoid/Avoid.h"
#include "Action/Move/ApproachTargetMove/ApproachTargetMove.h"
#include "Action/Move/ApproachLeaderMove/ApproachLeaderMove.h"
#include "Action/Move/NavMeshMove/NavMeshMove.h"
#include "Action/Move/NavMeshLeaderMove/NavMeshLeaderMove.h"
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

// アクションの種類
enum class ActionType
{
	// 攻撃系
	ComboAttack,
	GrabAttack,
	GrabStrikeAttack,

	// 回避系
	Avoid,

	// 移動系
	ApproachTargetMove,
	NavMeshMove,

	// 防御系
	Guard,

	// 攻撃動作判定系
	InAttackSequence,
	OutAttackSequence,

	// トークン系
	RequestToken,
	ReleaseToken,

	ApproachLeaderMove,
	NavMeshLeaderMove,
};

/// @brief アクションの種類を文字列で表す配列
constexpr inline const char* ACTION_TYPE_NAMES[] = {
	"ComboAttack",
	"GrabAttack",
	"GrabStrikeAttack",
	"Avoid",
	"ApproachTargetMove",
	"NavMeshMove",
	"Guard",
	"InAttackSequence",
	"OutAttackSequence",
	"RequestToken",
	"ReleaseToken",

	"ApproachLeaderMove",
	"NavMeshLeaderMove"
};

// 条件の種類
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

	IsTargetAttacking, // ターゲットが攻撃しているかどうか
	IsTargetNotAttacking, // ターゲットが攻撃していないかどうか

	IsTargetInAttackSequence, // ターゲットが攻撃動作中かどうか
	IsTargetNotInAttackSequence, // ターゲットが攻撃動作中でないかどうか

	IsClosestToTarget, // ターゲットに最も近いかどうか
	IsNotClosestToTarget, // ターゲットに最も近くないかどうか

	IsInAttackSequence, // 攻撃動作中かどうか
	IsNotInAttackSequence, // 攻撃動作中でないかどうか

	IsAvoiding, // 回避動作中かどうか
	IsNotAvoiding, // 回避動作中でないかどうか
};

// コンディションの種類を文字列で表す配列
constexpr inline const char* CONDITION_TYPE_NAMES[] = {
	"None",
	"HasTarget",
	"IsTargetDown",
	"IsNotTargetDown",
	"IsGrabbing",
	"IsNotGrabbing",
	"IsTargetInRange",
	"IsTargetOutOfRange",
	"IsTargetAttacking",
	"IsTargetNotAttacking",
	"IsTargetInAttackSequence",
	"IsTargetNotInAttackSequence",
	"IsClosestToTarget",
	"IsNotClosestToTarget",
	"IsInAttackSequence",
	"IsNotInAttackSequence",
	"IsAvoiding",
	"IsNotAvoiding"
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

	char name[128]; // ノードの名前
	Vector2 pos; // ノードの位置

	// アクションノードの場合のアクションの種類
	ActionType actionType = ActionType::ComboAttack;

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
	ApproachLeaderMoveInitData approachLeaderMoveInitData;
	NavMeshMoveInitData navMeshMoveInitData;
	NavMeshLeaderMoveInitData navMeshLeaderMoveInitData;

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