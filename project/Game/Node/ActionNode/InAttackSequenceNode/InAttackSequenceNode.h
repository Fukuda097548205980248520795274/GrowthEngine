#pragma once
#include "../ActionNode.h"
#include "Action/InAttackSequence/InAttackSequence.h"

class InAttackSequenceNode : public ActionNode
{
public:

	/// @brief コンストラクタ
    /// @param action 
    InAttackSequenceNode(std::unique_ptr<InAttackSequence> action)
        : ActionNode(std::move(action)) {
    }

    // Execをオーバーライドしてコンボ用の成功判定を行う
    Node::State Exec() override;

private:

    /// @brief 便利に扱うためのキャスト用プロパティ
    /// @return 
    InAttackSequence* GetInAttackSequence() { return static_cast<InAttackSequence*>(action_.get()); }
};

