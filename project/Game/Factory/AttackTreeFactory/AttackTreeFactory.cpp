#include "AttackTreeFactory.h"
#include "Entity/Character/Character.h"
#include "Builder/BehaviorTreeBuilder/BehaviorTreeBuilder.h"

/// @brief 実験用の攻撃ツリーを作成する
/// @param character 
/// @return 
std::unique_ptr<Node> AttackTreeFactory::CreateTestAttackTree(Character* character)
{
	// ビルダーを作成する
	BehaviorTreeBuilder builder;

	return builder
		.RestartingSelector()
			.RestartingSequence()
			.End()
		.Build();
}