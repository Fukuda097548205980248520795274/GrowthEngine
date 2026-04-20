#pragma once
#include "../Node.h"
#include "Attack/Attack.h"

class AttackNode : public Node
{
public:

	/// @brief コンストラクタ
	/// @param attack 
	AttackNode(std::unique_ptr<Attack> attack) : attack_(std::move(attack)), Node(){}

	/// @brief 実行
	/// @return 
	State Exec() override;


private:

	/// @brief 攻撃
	std::unique_ptr<Attack> attack_ = nullptr;
};

