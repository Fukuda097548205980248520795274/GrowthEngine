#include "RestartingSequenceNode.h"

/// @brief 実行
/// @return 
Node::State RestartingSequenceNode::Exec()
{
	// 子ノードがないと失敗
	if (children_.empty())
		return State::Failure;

	// 一つでも失敗したら終了
	for (std::unique_ptr<Node>& child : children_)
	{
		// 実行
		State state = child->Exec();

		// 失敗 or 実行中　最初から
		if (state == State::Failure || state == State::Running)
			return state;
	}

	return State::Success;
}