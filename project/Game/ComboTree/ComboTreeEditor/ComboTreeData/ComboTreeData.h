#pragma once
#include <string>
#include <vector>
#include "Action/Attack/ComboAttack/ComboAttack.h"
#include "Action/Attack/GrabAttack/GrabAttack.h"
#include "Action/Attack/GrabStrikeAttack/GrabStrikeAttack.h"
#include "Math/Vector/Vector2/Vector2.h"

// ノードの種類を表す列挙型
enum class ComboNodeType
{
	Combo,
	Grab,
	GrabStrike,
};


// ノード情報を保持する構造体
struct ComboEditorNode
{
	int id = 0;

	// 攻撃名
	std::string name{};

	// 遷移元から入ってくるピン
	int inputPinId = 0;

	// X入力派生用ピン
	int outputInputXPinId = 0;

	// B入力派生用ピン
	int outputInputBPinId = 0;

	// Y入力派生用ピン
	int outputInputYPinId = 0;

	/// @brief ノードの種類
	ComboNodeType nodeType = ComboNodeType::Combo;

	// エディタ上の座標
	ImVec2 pos;


	/// @brief コンボ攻撃の初期化データ
	CombAttackInitData comboAttackInitData;

	/// @brief つかみ攻撃の初期化データ
	GrabAttackInitData grabAttackInitData;

	/// @brief つかみ打撃攻撃の初期化データ
	GrabStrikeAttackInitData grabStrikeAttackInitData;


	// 攻撃アニメーション名
	std::string motionName;

	// GrabStrike用のターゲットアニメーション名
	std::string targetMotionName;
};

enum class ComboTreeInputType
{
	None,
	X,
	Y,
	B,
};

// リンク情報を保持する構造体
struct ComboEditorLink
{
	int id = 0;

	// 出力ピンのID
	int startPinId = 0;

	// 入力ピンのID
	int endPinId = 0;


	// 接続されている入力の種類（X, Y, B）
	ComboTreeInputType linkType = ComboTreeInputType::None;
};