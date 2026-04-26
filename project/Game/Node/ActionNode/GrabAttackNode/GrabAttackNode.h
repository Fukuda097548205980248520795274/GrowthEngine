#pragma once
#include "../ActionNode.h"
#include "Action/Attack/GrabAttack/GrabAttack.h"

class GrabAttackNode : public ActionNode
{
public:
    // ActionNodeのコンストラクタを利用
    GrabAttackNode(std::unique_ptr<GrabAttack> action)
        : ActionNode(std::move(action)) {
    }

    // Execをオーバーライドしてコンボ用の成功判定を行う
    Node::State Exec() override;

private:

    // 便利に扱うためのキャスト用プロパティ
    GrabAttack* GetGrabAttack() { return static_cast<GrabAttack*>(action_.get()); }
};

