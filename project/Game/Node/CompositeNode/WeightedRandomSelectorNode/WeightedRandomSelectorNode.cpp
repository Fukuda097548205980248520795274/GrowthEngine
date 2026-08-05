#include "WeightedRandomSelectorNode.h"

/// @brief 実行
Node::State WeightedRandomSelectorNode::Exec()
{
	if (children_.empty()) return State::Failure;

	int selectedIndex = -1;

	// 実行中のノードがあれば、それを優先して実行する
	if (runningChildIndex_ != -1)
	{
		selectedIndex = runningChildIndex_;
	}
	else
	{
		// 実行中のノードがない場合、重み付けによる抽選を行う
		float totalWeight = 0.0f;

		// 総重量の計算（前回実行したノードは除外する）
		for (size_t i = 0; i < children_.size(); ++i)
		{
			// 子ノードが複数ある場合のみ、前回実行したノードを抽選から外す
			if (static_cast<int>(i) == lastExecutedIndex_ && children_.size() > 1)
				continue;

			totalWeight += weights_[i];
		}

		// 有効な重みがない場合は失敗扱いとする
		if (totalWeight <= 0.0f) return State::Failure;

		// ランダム値を生成
		float randomValue = GetRandomRange(0.0f, totalWeight);

		// ランダム値に基づいてノードを選択
		for (size_t i = 0; i < children_.size(); ++i)
		{
			if (static_cast<int>(i) == lastExecutedIndex_ && children_.size() > 1)
				continue;

			randomValue -= weights_[i];
			if (randomValue <= 0.0f)
			{
				selectedIndex = static_cast<int>(i);
				break;
			}
		}

		// 万が一、選択されなかった場合は、前回実行したノード以外の最初のノードを選択する
		if (selectedIndex == -1)
		{
			selectedIndex = (lastExecutedIndex_ == 0 && children_.size() > 1) ? 1 : 0;
		}
	}

	// 選択されたノードを実行する
	State result = children_[selectedIndex]->UpdateNode();

	// 実行状態に応じたインデックスの更新
	if (result == State::Running)
	{
		// 実行中ならインデックスを保持
		runningChildIndex_ = selectedIndex;
	}
	else
	{
		// 実行が完了した場合はインデックスをリセットし、最後に実行したノードのインデックスを更新
		runningChildIndex_ = -1;
		lastExecutedIndex_ = selectedIndex;
	}

	return result;
}

/// @brief 中断処理
void WeightedRandomSelectorNode::Abort()
{
	// 実行中のノードがあれば中断させる
	if (runningChildIndex_ != -1 && runningChildIndex_ < children_.size())
	{
		children_[runningChildIndex_]->Abort();
		runningChildIndex_ = -1;
	}
}

/// @brief 子ノードを追加する（重み付き）
/// @param child 
/// @param weight 
void WeightedRandomSelectorNode::AddChildWithWeight(std::unique_ptr<Node> child, float weight)
{
	// 子ノードを追加
	AddChild(std::move(child));

	// 重みを追加
	weights_.push_back(weight);
}