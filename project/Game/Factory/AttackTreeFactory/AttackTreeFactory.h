#pragma once
#include "Node/Node.h"

class Character;

class AttackTreeFactory
{
public:

   /// @brief 実験用のツリーを作成する
	/// @param character 
	/// @return 
   std::unique_ptr<Node> CreateTestTree(Character* character);
};

