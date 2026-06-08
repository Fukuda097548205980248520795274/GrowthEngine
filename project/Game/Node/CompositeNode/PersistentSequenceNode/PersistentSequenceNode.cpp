#include "PersistentSequenceNode.h"

/// @brief 実行
/// @return 
Node::State PersistentSequenceNode::Exec()
{
	// 子ノードがないと失敗
	if (children_.empty())
		return State::Failure;

	// 一つでも失敗したら終了
	while (currentIndex_ < children_.size())
	{
		// 実行
		State state = children_[currentIndex_]->UpdateNode();

		// 成功　次の子ノードへ進む
		if (state == State::Success)
			currentIndex_++;

		// 実行中 その場で終了
		if (state == State::Running)
			return state;

		// 失敗　最初から
		if (state == State::Failure)
		{
			// それ以降のノードは中断する
			Abort();
			return state;
		}
	}

	// 成功
	currentIndex_ = 0;
	return State::Success;
}

/// @brief 中断処理
void PersistentSequenceNode::Abort()
{
	// 自分自身の記憶（インデックス）をリセットする
	currentIndex_ = 0;

	// 子ノードたちにも中断を伝える
	CompositeNode::Abort();
}