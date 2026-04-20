#pragma once
#include "Node/Node.h"

class Character;

class AttackTreeFactory
{
public:

	/// @brief 実験用の攻撃ツリーを作成する
	/// @param character 
	/// @return 
	std::unique_ptr<Node> CreateTestAttackTree(Character* character);
};

