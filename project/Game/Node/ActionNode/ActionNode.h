#pragma once
#include "../Node.h"
#include "Action/Action.h"

class ActionNode : public Node
{
public:

	/// @brief コンストラクタ
	/// @param action
	ActionNode(std::unique_ptr<Action> action) : action_(std::move(action)), Node(){}

	/// @brief 実行
	/// @return 
	State Exec() override;

	/// @brief 中断処理
	virtual void Abort() override;


protected:

	/// @brief アクション
	std::unique_ptr<Action> action_ = nullptr;
};

