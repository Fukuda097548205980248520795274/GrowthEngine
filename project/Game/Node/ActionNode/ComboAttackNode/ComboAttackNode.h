#pragma once
#include "../ActionNode.h"
#include "Action/Attack/ComboAttack/ComboAttack.h"

class ComboAttackNode : public ActionNode
{
public:
    // ActionNodeのコンストラクタを利用
    ComboAttackNode(std::unique_ptr<ComboAttack> action)
        : ActionNode(std::move(action)) {
    }

    // Execをオーバーライドしてコンボ用の成功判定を行う
    Node::State Exec() override;

private:
    // 便利に扱うためのキャスト用プロパティ
    ComboAttack* GetComboAttack() { return static_cast<ComboAttack*>(action_.get()); }
};

