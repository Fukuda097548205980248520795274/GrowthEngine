#pragma once
#include "../Node.h"

class ConditionNode : public Node
{
public:

	/// @brief コンストラクタ
	/// @param func 
	ConditionNode(std::function<bool()> func) : Node(), func_(func) {}

	/// @brief 実行
	/// @return ノードの状態
	State Exec() override;

private:

	/// @brief 条件関数
	std::function<bool()> func_;
};

