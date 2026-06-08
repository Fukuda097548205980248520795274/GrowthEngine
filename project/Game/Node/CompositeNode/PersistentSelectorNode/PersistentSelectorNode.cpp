#include "PersistentSelectorNode.h"

/// @brief 実行
/// @return 
Node::State PersistentSelectorNode::Exec()
{
	// 子ノードがないと失敗
	if (children_.empty())
		return State::Failure;

	// 子ノードが一つ成功するまで
	while (currentIndex_ < children_.size())
	{
		// 実行
		State state = children_[currentIndex_]->UpdateNode();

		// 成功　次の子ノードへ進む
		if (state == State::Success)
		{
			currentIndex_ = 0;
			return state;
		}

		// 実行中 その場で終了
		if (state == State::Running)
			return state;

		// 失敗　最初から
		if (state == State::Failure)
			currentIndex_++;
	}

	// 一つも成功しないので失敗
	currentIndex_ = 0;
	return State::Failure;
}

/// @brief 中断処理
void PersistentSelectorNode::Abort()
{
	// 自分自身の記憶（インデックス）をリセットする
	currentIndex_ = 0;

	// 子ノードたちにも中断を伝える
	CompositeNode::Abort();
}