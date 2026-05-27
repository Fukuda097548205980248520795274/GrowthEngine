#include "ComboAttackNode.h"

Node::State ComboAttackNode::Exec()
{
	// アクションがない場合は失敗
    ComboAttack* combo = GetComboAttack();
    if (!combo) return State::Failure;

	// アクションが実行されていない場合は実行する
    if (!combo->IsExec() && !combo->IsSuccess() && !combo->IsFailure())
        combo->Exec();

	// アクションが失敗していて、かつ使用中でない場合は失敗とする
	if (combo->IsFailure() && !combo->IsUse())
	{
		combo->Reset();
		return State::Failure;
	}

	// コンボキャンセル可能な状態なら成功とする
	if (combo->IsCanNextCombo())
	{
		combo->Reset();
		return State::Success;
	}
    
	// アクションの状態に応じてノードの状態を返す
	State result = State::Failure;
	if (combo->IsSuccess() ||combo->IsFailure())
	{
		// アクションの状態に応じてノードの状態を返す
		result = combo->IsSuccess() ? State::Success : State::Failure;
		action_->Reset();
	}
	else
	{
		result = State::Running;
	}

	return result;
}