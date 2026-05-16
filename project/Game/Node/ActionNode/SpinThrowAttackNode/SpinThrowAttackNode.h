#pragma once
#include "../ActionNode.h"
#include "Action/Attack/SpinThrowAttack/SpinThrowAttack.h"

class SpinThrowAttackNode : public ActionNode
{
public:
    
	/// @brief コンストラクタ
    /// @param action 
    SpinThrowAttackNode(std::unique_ptr<SpinThrowAttack> action)
        : ActionNode(std::move(action)) {
    }

    // Execをオーバーライドしてコンボ用の成功判定を行う
    Node::State Exec() override;

private:

    // 便利に扱うためのキャスト用プロパティ
    SpinThrowAttack* GetSpinThrowAttack() { return static_cast<SpinThrowAttack*>(action_.get()); }
};

