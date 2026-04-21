#include "ActionNode.h"

/// @brief 実行
/// @return 
Node::State ActionNode::Exec()
{
	// 攻撃がない場合は失敗
	if (!action_)return State::Failure;

	// 実行する
	action_->Exec();

	if(action_->IsExec())
	{
		// 実行中の場合は実行中
		return State::Running;
	}
	else
	{
		// 実行終了後

		// 成功している場合は成功
		if (action_->IsSuccess())
			return State::Success;

		// 失敗している場合は失敗
		if(action_->IsFailure())
			return State::Failure;
	}

	// ここには来ないはず
	assert(false);
	return State::Running;
}