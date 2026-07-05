#pragma once
#include <string>
#include <vector>
#include "Action/Attack/ComboAttack/ComboAttack.h"
#include "Action/Attack/GrabAttack/GrabAttack.h"
#include "Math/Vector/Vector2/Vector2.h"

// ノードの種類を表す列挙型
enum class ComboNodeType
{
    Combo,
    Grab
};


// ノード情報を保持する構造体
struct ComboEditorNode
{
    int id = 0;

    // 攻撃名
    char name[128] = "Combo Attack";

    // 遷移元から入ってくるピン
    int inputPinId = 0;

    // 弱攻撃（Light）派生用ピン
    int outputLightPinId = 0;

    // 強攻撃（Heavy）派生用ピン
    int outputHeavyPinId = 0;

	/// @brief ノードの種類
    ComboNodeType nodeType = ComboNodeType::Combo;

    // エディタ上の座標
    ImVec2 pos;


	/// @brief コンボ攻撃の初期化データ
	CombAttackInitData comboAttackInitData;

	/// @brief つかみ攻撃の初期化データ
	GrabAttackInitData grabAttackInitData;


	// 攻撃アニメーション名
    std::string motionName;
};

// リンク情報を保持する構造体
struct ComboEditorLink
{
    int id = 0;

    // 出力ピンのID
    int startPinId = 0;

    // 入力ピンのID
    int endPinId = 0;


	// リンクの種類（1: 弱攻撃, 2: 強攻撃）
    int linkType = 0;
};