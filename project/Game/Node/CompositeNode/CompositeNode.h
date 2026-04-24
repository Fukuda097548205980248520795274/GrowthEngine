#pragma once
#include "../Node.h"

class CompositeNode : public Node
{
public:

	/// @brief コンストラクタ
	CompositeNode() : Node() {}

	/// @brief 実行
	/// @return ノードの状態
	virtual State Exec() override = 0;

	/// @brief 子ノードを追加する
	/// @param child 
	void AddChild(std::unique_ptr<Node> child) { children_.push_back(std::move(child)); }

	/// @brief 中断処理
	virtual void Abort() override;


protected:

	/// @brief 子ノードたち
	std::vector<std::unique_ptr<Node>> children_;
};

