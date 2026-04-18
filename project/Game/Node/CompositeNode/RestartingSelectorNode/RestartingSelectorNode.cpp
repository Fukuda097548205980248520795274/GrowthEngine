#include "RestartingSelectorNode.h"

/// @brief 実行
/// @return 
Node::State RestartingSelectorNode::Exec()
{
	// 子ノードがないと失敗
	if (children_.empty())
		return State::Failure;

	// 子ノードが一つ成功するまで
	for (std::unique_ptr<Node>& child : children_)
	{
		// 実行
		State state = child->Exec();

		// 成功 or 実行中
		if (state == State::Success || state == State::Running)
			return state;
	}

	// 一つも成功しないので失敗
	return State::Failure;
}