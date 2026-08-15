#pragma once
#include "../Node.h"

class Character;

class ConditionNode : public Node
{
public:

	/// @brief コンストラクタ
	/// @param owner 
	/// @param func 
	ConditionNode(Character* owner, std::function<bool(Character* owner)> func) : Node(), func_(func), owner_(owner) {}

	/// @brief 実行
	/// @return ノードの状態
	State Exec() override;

	/// @brief 所有者を設定する
	/// @param owner 
	void SetOwner(Character* owner) override { owner_ = owner; }

private:

	/// @brief 条件関数
	std::function<bool(Character* owner)> func_;

	/// @brief 所有者
	Character* owner_ = nullptr;
};

