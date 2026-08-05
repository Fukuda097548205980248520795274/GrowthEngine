#pragma once
#include "../CompositeNode.h"

class WeightedRandomSelectorNode : public CompositeNode
{
public:

    /// @brief コンストラクタ
    WeightedRandomSelectorNode() : CompositeNode() {}

    /// @brief 実行
    /// @return 
    State Exec() override;

    /// @brief 中断処理
    void Abort() override;

	/// @brief 子ノードを重み付きで追加する
    /// @param child 
    /// @param weight 
    void AddChildWithWeight(std::unique_ptr<Node> child, float weight);

private:

    /// @brief 各子ノードの重み
    std::vector<float> weights_;

    /// @brief 実行中の子ノードのインデックス
    int runningChildIndex_ = -1;

    /// @brief 前回実行を完了した子ノードのインデックス（連続実行防止用）
    int lastExecutedIndex_ = -1;
};

