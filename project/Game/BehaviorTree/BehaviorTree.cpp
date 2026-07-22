#include "BehaviorTree.h"

/// @brief 実行
BehaviorTree::State BehaviorTree::Exec()
{
#ifdef DEVELOPMENT
	root_->ResetStatusRecursive();
#endif

	Node::State nodeState = Node::State::None;

	// ノードの実行
	if (root_)nodeState = root_->UpdateNode();

	// ノードの状態をツリーの状態に変換
	switch (nodeState)
	{
	case Node::State::Success:
		currentState_ = State::Success;
		break;

	case Node::State::Failure:
		currentState_ = State::Failure;
		break;

	case Node::State::Running:
		currentState_ = State::Running;
		break;
	}

	return currentState_;
}