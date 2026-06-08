#include "RestartingSelectorNode.h"

/// @brief 実行
/// @return 
Node::State RestartingSelectorNode::Exec()
{
	// 子ノードがないと失敗
	if (children_.empty())
		return State::Failure;

	// 子ノードが一つ成功するまで
    for (size_t i = 0; i < children_.size(); ++i)
    {
        State state = children_[i]->UpdateNode();

        // 成功、あるいは実行中のノードが見つかった場合
        if (state == State::Success || state == State::Running)
        {
			// それ以降のノードは中断する
            for (size_t j = i + 1; j < children_.size(); ++j)
                children_[j]->Abort();

            return state;
        }
    }

	// 一つも成功しないので失敗
	return State::Failure;
}