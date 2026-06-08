#include "InAttackSequenceNode.h"

Node::State InAttackSequenceNode::Exec()
{
	// アクションがない場合は失敗
	InAttackSequence* sequence = GetInAttackSequence();
	if (!sequence)return State::Failure;

	// 実行と更新
	sequence->Exec();

	// 必ず成功しなければいけいない
	return State::Success;
}