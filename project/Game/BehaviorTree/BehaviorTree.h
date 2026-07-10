#pragma once
#include "Node/Node.h"

class BehaviorTree
{
public:

	/// @brief コンストラクタ
	/// @param root ルートノード
	BehaviorTree(std::unique_ptr<Node> root) : root_(std::move(root)) {}

	/// @brief 仮想デストラクタ
	virtual ~BehaviorTree() = default;

	/// @brief 実行
	void Exec();

	/// @brief ルートノードを取得する
	/// @return 
	Node* GetRoot() const { return root_.get(); }

protected:

	/// @brief ルートノード
	std::unique_ptr<Node> root_ = nullptr;
};

