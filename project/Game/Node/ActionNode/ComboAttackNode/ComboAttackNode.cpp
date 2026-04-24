#include "ComboAttackNode.h"

Node::State ComboAttackNode::Exec()
{
	// アクションがない場合は失敗
    ComboAttack* combo = GetComboAttack();
    if (!combo) return State::Failure;

	// アクションが実行されていない場合は実行する
    if (!combo->IsExec() && !combo->IsSuccess() && !combo->IsFailure())
        combo->Exec();

	// 失敗したら失敗とする（コンボが失敗したら次の攻撃に進めないため）
	if (combo->IsFailure())
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
		// アクションをリセットする
		action_->Reset();
	}
	else
	{
		result = State::Running;
	}

	return result;
}