#include "UtilitySelectorNode.h"

/// @brief 実行
Node::State UtilitySelectorNode::Exec()
{
    if (children_.empty()) return State::Failure;

    int bestIndex = -1;

	// 実行中のノードがあれば、それを優先して実行する
    if (runningChildIndex_ != -1)
    {
        bestIndex = runningChildIndex_;
    }
    else
    {
		// 実行中のノードがない場合、ユーティリティ関数を使って最もスコアの高いノードを選択する
        float highestScore = -1.0f;

        // 全ての子ノードのスコアを計算し、最高のものを探す
        for (size_t i = 0; i < children_.size(); ++i)
        {
            if (!utilityFunctions_[i]) continue;

            float score = utilityFunctions_[i]();
            if (score > highestScore)
            {
                highestScore = score;
                bestIndex = static_cast<int>(i);
            }
        }
    }

	// 最もスコアの高いノードが見つからなかった場合、失敗を返す
    if (bestIndex == -1) return State::Failure;

	// 選択されたノードを実行する
    State result = children_[bestIndex]->UpdateNode();

	// 実行中のノードがある場合、そのインデックスを保持する
    if (result == State::Running)
    {
        runningChildIndex_ = bestIndex;
    }
    else
    {
        runningChildIndex_ = -1;
    }

    return result;
}

/// @brief 中断処理
void UtilitySelectorNode::Abort()
{
    // 実行中のノードがあれば中断させる
    if (runningChildIndex_ != -1 && runningChildIndex_ < children_.size())
    {
        children_[runningChildIndex_]->Abort();
        runningChildIndex_ = -1;
    }
}

/// @brief 子ノードを追加する（ユーティリティ関数付き）
/// @param child 
/// @param utilityFunc 
void UtilitySelectorNode::AddChildWithUtility(std::unique_ptr<Node> child, std::function<float()> utilityFunc)
{
	// 子ノードを追加
    AddChild(std::move(child));

	// ユーティリティ関数を追加
    utilityFunctions_.push_back(utilityFunc);
}
