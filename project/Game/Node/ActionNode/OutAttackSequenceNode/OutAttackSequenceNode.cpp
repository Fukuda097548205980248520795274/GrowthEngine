#include "OutAttackSequenceNode.h"

Node::State OutAttackSequenceNode::Exec()
{
	// アクションがない場合は失敗
	OutAttackSequence* sequence = GetOutAttackSequence();
	if (!sequence)return State::Failure;

	// 実行と更新
	sequence->Exec();

	return State::Success;
}

/// @brief 中断処理
void OutAttackSequenceNode::Abort()
{
	OutAttackSequence* sequence = GetOutAttackSequence();
	if (!sequence)return;

	// 実行と更新
	sequence->Exec();
}