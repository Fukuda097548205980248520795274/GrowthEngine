#include "AttackNode.h"

/// @brief 実行
/// @return 
Node::State AttackNode::Exec()
{
	// 攻撃がない場合は失敗
	if (!attack_)return State::Failure;

	// 攻撃を実行する
	attack_->Exec();

	if(attack_->IsExec())
	{
		// 攻撃が実行中の場合は実行中
		return State::Running;
	}
	else
	{
		// 攻撃終了後

		// 成功している場合は成功
		if (attack_->IsSuccess())
			return State::Success;

		// 失敗している場合は失敗
		if(attack_->IsFailure())
			return State::Failure;
	}

	// ここには来ないはず
	assert(false);
	return State::Running;
}