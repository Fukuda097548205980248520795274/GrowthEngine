#include "AvoidNode.h"

Node::State AvoidNode::Exec()
{
	// アクションがない場合は失敗
	Avoid* avoid = GetAvoid();
	if (!avoid) return State::Failure;

	// アクションが実行されていない場合は実行する
	if (!avoid->IsExec() && !avoid->IsSuccess() && !avoid->IsFailure())
		avoid->Exec();

	// アクションが失敗していて、かつ使用中でない場合は失敗とする
	if (avoid->IsFailure() && !avoid->IsUse())
	{
		avoid->Reset();
		return State::Failure;
	}

	// アクションの状態に応じてノードの状態を返す
	State result = State::Failure;
	if (avoid->IsSuccess() || avoid->IsFailure())
	{
		// アクションの状態に応じてノードの状態を返す
		result = avoid->IsSuccess() ? State::Success : State::Failure;
		action_->Reset();
	}
	else
	{
		result = State::Running;
	}

	return result;
}