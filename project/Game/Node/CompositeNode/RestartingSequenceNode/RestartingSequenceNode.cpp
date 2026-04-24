#include "RestartingSequenceNode.h"

/// @brief 実行
/// @return 
Node::State RestartingSequenceNode::Exec()
{
	// 子ノードがないと失敗
	if (children_.empty())
		return State::Failure;

	// 一つでも失敗したら終了
    for (size_t i = 0; i < children_.size(); ++i)
    {
        State state = children_[i]->Exec();

        // 失敗、あるいは実行中のノードが見つかってここで止まる場合
        if (state == State::Failure || state == State::Running)
        {
			// それ以降のノードは中断する
            for (size_t j = i + 1; j < children_.size(); ++j)
                children_[j]->Abort();

            return state;
        }
    }

	return State::Success;
}