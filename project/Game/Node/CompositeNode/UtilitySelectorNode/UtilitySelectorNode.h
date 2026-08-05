#pragma once
#include "../CompositeNode.h"

class UtilitySelectorNode : public CompositeNode
{
public:

    /// @brief コンストラクタ
    UtilitySelectorNode() : CompositeNode() {}

	/// @brief 実行
    /// @return 
    State Exec() override;

	/// @brief 中断処理
    void Abort() override;

	/// @brief 子ノードを追加する（ユーティリティ関数付き）
    /// @param child 
    /// @param utilityFunc 
    void AddChildWithUtility(std::unique_ptr<Node> child, std::function<float()> utilityFunc);


private:

	/// @brief ユーティリティ関数たち
    std::vector<std::function<float()>> utilityFunctions_;

	/// @brief 実行する子ノードのインデックス
    int runningChildIndex_ = -1;
};

