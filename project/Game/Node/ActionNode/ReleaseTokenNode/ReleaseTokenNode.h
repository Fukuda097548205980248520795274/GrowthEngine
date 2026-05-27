#pragma once
#include "../ActionNode.h"
#include "Action/ReleaseToken/ReleaseToken.h"

class ReleaseTokenNode : public ActionNode
{
public:
    // ActionNodeのコンストラクタを利用
    ReleaseTokenNode(std::unique_ptr<ReleaseToken> action)
        : ActionNode(std::move(action)) {
    }

    // Execをオーバーライドしてコンボ用の成功判定を行う
    Node::State Exec() override;

	/// @brief 中断処理
	void Abort() override;

private:

    /// @brief 便利に扱うためのキャスト用プロパティ
    /// @return 
    ReleaseToken* GetReleaseToken() { return static_cast<ReleaseToken*>(action_.get()); }
};

