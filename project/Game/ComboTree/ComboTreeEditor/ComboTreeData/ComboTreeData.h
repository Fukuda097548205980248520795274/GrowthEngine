#pragma once
#include <string>
#include <vector>
#include "Action/Attack/Attack.h"
#include "Math/Vector/Vector2/Vector2.h"


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

    // エディタ上の座標
    ImVec2 pos;


    // 攻撃全体の時間
    float attackTime = 1.0f;

    // 移動速度
    float moveSpeed = 0.0f;

    // 移動開始時間
    float moveStartTime = 0.0f;

    // 移動終了時間
    float moveEndTime = 0.0f;

    // キャンセル可能開始時間
    float cancelStartTime = 0.0f;

    // キャンセル可能終了時間
    float cancelEndTime = 0.0f;


	// 攻撃アニメーション名
    std::string motionName;


	// 当たり判定の定義リスト
    std::vector<HitboxDefinition> hitDefinitions;
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