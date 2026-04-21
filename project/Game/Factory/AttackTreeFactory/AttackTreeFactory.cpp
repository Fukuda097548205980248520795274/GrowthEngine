#include "AttackTreeFactory.h"
#include "Entity/Character/Character.h"
#include "Builder/BehaviorTreeBuilder/BehaviorTreeBuilder.h"
#include "Action/Move/ApproachTargetMove/ApproachTargetMove.h"

/// @brief 実験用のツリーを作成する
/// @param character 
/// @return 
std::unique_ptr<Node> AttackTreeFactory::CreateTestTree(Character* character)
{
	// ビルダーを作成する
	BehaviorTreeBuilder builder;

	return builder
       .RestartingSequence()
			.Condition([character]()
				{
					return character->GetLockOnTarget() != nullptr;
				})
			.End()
			.Action_(std::make_unique<ApproachTargetMove>(character, 1.0f, 3.0f))
			.End()
		.End()
		.Build();
}
