#include "ActionNode.h"

/// @brief 実行
/// @return 
Node::State ActionNode::Exec()
{
	// アクションがない場合は失敗
	if (!action_) return State::Failure;

	
	// アクションが実行されていない場合は実行する
	if (!action_->IsExec() && !action_->IsSuccess() && !action_->IsFailure())
		action_->Exec();

	// アクションが実行中の場合は、ノードの状態も実行中
	if (action_->IsExec())
		return State::Running;

	// アクションの状態に応じてノードの状態を返す
	State result = State::Failure;
	if (action_->IsSuccess())
	{
		result = State::Success;
	} 
	else if (action_->IsFailure())
	{
		result = State::Failure;
	}

	// アクションをリセットする
	action_->Reset();
	return result;
}

/// @brief 中断処理
void ActionNode::Abort()
{
	// アクションが動いていたら強制終了
	if (action_ && action_->IsExec())
		action_->Exit();
}