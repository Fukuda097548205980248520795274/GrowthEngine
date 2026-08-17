#pragma once
#include "GrowthEngine.h"

#include "Action/Attack/ComboAttack/ComboAttack.h"
#include "Action/Attack/GrabAttack/GrabAttack.h"
#include "Action/Attack/GrabStrikeAttack/GrabStrikeAttack.h"
#include "Action/Avoid/Avoid.h"
#include "Action/Defense/Defense.h"
#include "Action/Move/NavMeshMove/NavMeshMove.h"
#include "Action/Move/NavMeshLeaderMove/NavMeshLeaderMove.h"
#include "Action/Telegraph/Telegraph.h"
#include "Action/RequestToken/RequestToken.h"
#include "Action/ReleaseToken/ReleaseToken.h"
#include "Action/InAttackSequence/InAttackSequence.h"
#include "Action/OutAttackSequence/OutAttackSequence.h"
#include "MotionManager/MotionManager.h"
#include "BattleDirector/BattleDirector.h"

// ノードの種類
enum class EditorNodeType
{
	PersistentSelector,
	PersistentSequence,
	RestartingSelector,
	RestartingSequence,
	UtilitySelector = 6,
	WeightedRandomSelector,
	Condition = 4,
	Action,
	SubTree = 8,
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
	NavMeshMove = 5,

	// 防御系
	Guard,

	// 攻撃動作判定系
	InAttackSequence,
	OutAttackSequence,

	// トークン系
	RequestToken,
	ReleaseToken,

	NavMeshLeaderMove = 12,

	// 予備動作
	Telegraph,

	// 防御
	Defense,
};

/// @brief アクションの種類を文字列で表す配列
constexpr inline const char* actionTypeNames[] = 
{
	"ComboAttack",
	"GrabAttack",
	"GrabStrikeAttack",
	"Avoid",
	"",
	"NavMeshMove",
	"Guard",
	"InAttackSequence",
	"OutAttackSequence",
	"RequestToken",
	"ReleaseToken",

	"",
	"NavMeshLeaderMove",
	"Telegraph",
	"Defense"
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

	IsDamageReaction, // ダメージリアクション中かどうか
	IsNotDamageReaction, // ダメージリアクション中でないかどうか

	IsChangeState, // 状態遷移中かどうか
	IsNotChangeState, // 状態遷移中でないかどうか
};

// コンディションの種類を文字列で表す配列
constexpr inline const char* conditionTypeNames[] = {
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
	"IsNotAvoiding",
	"IsDamageReaction",
	"IsNotDamageReaction",
	"IsChangeState",
	"IsNotChangeState",
};

/// @brief ユーティリティの種類
enum class UtilityType
{
	FixedDefault,
	HpRatio,
};

/// @brief ユーティリティの種類を文字列で表す配列
constexpr inline const char* utilityTypeNames[] = {
	"FixedDefault",
	"HpRatio",
};

/// @brief トークンの種類を文字列で表す配列
constexpr inline const char* tokenTypeNames[] = {
	"攻撃",
	"挑発",
	"フェイント"
};

/// @brief 条件ノードパラメータ
struct ConditionNodeParam
{
	float distanceToTarget = 0.0f; // ターゲットとの距離
};

// エディタ上のノードを表す構造体
struct EditorNode
{
	int id; // ノードのID
	EditorNodeType type; // ノードの種類

	// 入力ピンのID（親とつなげる）
	int inputPinId;

	// 出力ピンのID（子とつなげる）
	int outputPinId;

	// サブツリーノードの場合のサブツリーファイル名
	char subTreeFileName[64] = "";

	char name[128]; // ノードの名前
	Vector2 pos; // ノードの位置

	// アクションノードの場合のアクションの種類
	ActionType actionType = ActionType::ComboAttack;

	// 条件ノードの場合の条件の種類
	ConditionType conditionType = ConditionType::None;

	// アクションノードの場合のトークンの種類
	ActionTokenType tokenType = ActionTokenType::Attack;

	MotionType motionType = MotionType::Stand; // 条件ノードでモーションを条件にする場合のモーションの種類
	std::string motionName{};

	MotionType targetMotionType = MotionType::Stand; // 条件ノードでターゲットのモーションを条件にする場合のモーションの種類
	std::string targetMotionName{};

	// Utilityセレクタノードの場合の子ノードごとの評価関数の種類を保持するマップ
	std::unordered_map<int, UtilityType> childUtilityMap;

	// WeightedRandomSelectorノードの場合の子ノードごとの重みを保持するマップ
	std::unordered_map<int, float> childWeightMap;

	// 初期化用データ（アクションノードの種類に応じて使用）
	CombAttackInitData comboAttackInitData;
	GrabAttackInitData grabAttackInitData;
	GrabStrikeAttackInitData grabStrikeAttackInitData;
	AvoidInitData avoidInitData;
	NavMeshMoveInitData navMeshMoveInitData;
	NavMeshLeaderMoveInitData navMeshLeaderMoveInitData;
	TelegraphInitData telegraphInitData;
	DefenseInitData defenseInitData;

	// 条件ノードのパラメータ
	ConditionNodeParam conditionParam;

	// ノードが展開されているかどうかを示すフラグ
	bool isCollapsed = false;

	// ノードの位置を次の描画フレームでImNodesに反映する必要があるかどうかを示すフラグ
	bool needSetPos = true;
};

// エディタ上のリンクを表す構造体
struct EditorLink
{
	// リンクのID
	int id;

	// リンクの開始ピンのID（親）
	int startPinId;

	// リンクの終了ピンのID（子）
	int endPinId;

	// 入力ピンの親ノードID
	int startNodeId;

	// 出力ピンの子ノードID
	int endNodeId;
};