#pragma once
#include "../ActionNode.h"
#include "Action/Avoid/Avoid.h"

class AvoidNode : public ActionNode
{
public:
    // ActionNodeのコンストラクタを利用
    AvoidNode(std::unique_ptr<Avoid> action)
        : ActionNode(std::move(action)) {
    }

    // Execをオーバーライドしてコンボ用の成功判定を行う
    Node::State Exec() override;

private:

    // 便利に扱うためのキャスト用プロパティ
    Avoid* GetAvoid() { return static_cast<Avoid*>(action_.get()); }
};

