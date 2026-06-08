#pragma once
#include "../ActionNode.h"
#include "Action/OutAttackSequence/OutAttackSequence.h"

class OutAttackSequenceNode : public ActionNode
{
public:
    // ActionNodeのコンストラクタを利用
    OutAttackSequenceNode(std::unique_ptr<OutAttackSequence> action)
        : ActionNode(std::move(action)) {
    }

    // Execをオーバーライドしてコンボ用の成功判定を行う
    Node::State Exec() override;

    /// @brief 中断処理
    void Abort() override;

private:

    /// @brief 便利に扱うためのキャスト用プロパティ
    /// @return 
    OutAttackSequence* GetOutAttackSequence() { return static_cast<OutAttackSequence*>(action_.get()); }
};

