#include "GrabAttackNode.h"

Node::State GrabAttackNode::Exec()
{
	// アクションがない場合は失敗
	GrabAttack* grab = GetGrabAttack();
	if (!grab) return State::Failure;

	// アクションが実行されていない場合は実行する
	if (!grab->IsExec() && !grab->IsSuccess() && !grab->IsFailure())
		grab->Exec();

	// アクションが失敗している、あるいは使用されていない場合は失敗
	if (grab->IsFailure() || !grab->IsUse())
	{
		grab->Reset();
		return State::Failure;
	}

	// アクションの状態に応じてノードの状態を返す
	State result = State::Failure;
	if (grab->IsSuccess() || grab->IsFailure())
	{
		if(grab->IsSuccess())
			result = State::Success;

		if(grab->IsFailure())
			result = State::Failure;

		// アクションをリセットする
		grab->Reset();
	}
	else
	{
		result = State::Running;
	}

	return result;
}