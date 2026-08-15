#include "ConditionNode.h"

/// @brief 実行
/// @return ノードの状態
Node::State ConditionNode::Exec()
{
	// 条件関数がない場合は失敗とする
	if (!func_ || !owner_)return State::Failure;

	// 条件関数の結果がtrueなら成功
	if(func_(owner_))return State::Success;
	
	// 条件関数の結果がfalseなら失敗
	return State::Failure;
}