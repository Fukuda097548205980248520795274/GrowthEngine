#pragma once
#include "../ActionNode.h"
#include "Action/Attack/GrabStrikeAttack/GrabStrikeAttack.h"

class GrabStrikeAttackNode : public ActionNode
{
public:
    // ActionNodeのコンストラクタを利用
    GrabStrikeAttackNode(std::unique_ptr<GrabStrikeAttack> action)
        : ActionNode(std::move(action)) {
    }

    // Execをオーバーライドしてコンボ用の成功判定を行う
    Node::State Exec() override;

private:

    // 便利に扱うためのキャスト用プロパティ
    GrabStrikeAttack* GetGrabStrikeAttack() { return static_cast<GrabStrikeAttack*>(action_.get()); }
};

