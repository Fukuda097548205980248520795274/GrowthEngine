#pragma once
#include <vector>
#include <memory>
#include "Action/Attack/ComboAttack/ComboAttack.h"

class ComboTree
{
public:

    // 生成されたすべての攻撃ステートのメモリを管理
    std::vector<std::unique_ptr<ComboAttack>> allAttacks;

    // コンボの始点となる最初の一撃のポインタ（Playerはこれを呼び出す）
    ComboAttack* rootAttack = nullptr;
};

