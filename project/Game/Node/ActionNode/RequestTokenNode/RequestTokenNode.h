#pragma once
#include "../ActionNode.h"
#include "Action/RequestToken/RequestToken.h"

class RequestTokenNode : public ActionNode
{
public:
    // ActionNodeのコンストラクタを利用
    RequestTokenNode(std::unique_ptr<RequestToken> action)
        : ActionNode(std::move(action)) {
    }

    // Execをオーバーライドしてコンボ用の成功判定を行う
    Node::State Exec() override;

private:

	/// @brief 便利に扱うためのキャスト用プロパティ
    /// @return 
    RequestToken* GetRequestToken() { return static_cast<RequestToken*>(action_.get()); }
};

