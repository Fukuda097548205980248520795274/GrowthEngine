#pragma once
#include "../CompositeNode.h"

class Character;

class UtilitySelectorNode : public CompositeNode
{
public:

    /// @brief コンストラクタ
    UtilitySelectorNode(Character* owner) : CompositeNode(), owner_(owner) {}

	/// @brief 実行
    /// @return 
    State Exec() override;

	/// @brief 中断処理
    void Abort() override;

	/// @brief 子ノードを追加する（ユーティリティ関数付き）
    /// @param child 
    /// @param utilityFunc 
    void AddChildWithUtility(std::unique_ptr<Node> child, std::function<float(Character*)> utilityFunc);

	/// @brief 所有者を設定する
	/// @param owner 
	void SetOwner(Character* owner) override { owner_ = owner; }


private:

	/// @brief 所有者
	Character* owner_ = nullptr;

	/// @brief ユーティリティ関数たち
    std::vector<std::function<float(Character*)>> utilityFunctions_;

	/// @brief 実行する子ノードのインデックス
    int runningChildIndex_ = -1;
};

