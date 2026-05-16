#include "SpinThrowAttackNode.h"

Node::State SpinThrowAttackNode::Exec()
{
	// アクションがない場合は失敗
	SpinThrowAttack* spinThrow = GetSpinThrowAttack();
	if (!spinThrow) return State::Failure;

	// アクションが実行されていない場合は実行する
	if (!spinThrow->IsExec() && !spinThrow->IsSuccess() && !spinThrow->IsFailure())
		spinThrow->Exec();

	// アクションが失敗している、あるいは使用されていない場合は失敗
	if (spinThrow->IsFailure() || !spinThrow->IsUse())
	{
		spinThrow->Reset();
		return State::Failure;
	}

	// アクションの状態に応じてノードの状態を返す
	State result = State::Failure;
	if (spinThrow->IsSuccess() || spinThrow->IsFailure())
	{
		if (spinThrow->IsSuccess())
			result = State::Success;

		if (spinThrow->IsFailure())
			result = State::Failure;

		// アクションをリセットする
		spinThrow->Reset();
	}
	else
	{
		result = State::Running;
	}

	return result;
}