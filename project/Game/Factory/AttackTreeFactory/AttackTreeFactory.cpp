#include "AttackTreeFactory.h"
#include "Entity/Character/Character.h"
#include "Builder/BehaviorTreeBuilder/BehaviorTreeBuilder.h"
#include "Action/Move/ApproachTargetMove/ApproachTargetMove.h"
#include "Action/Avoid/Avoid.h"

/// @brief 実験用のツリーを作成する
/// @param character 
/// @return 
std::unique_ptr<Node> AttackTreeFactory::CreateTestTree(Character* character)
{
	// ビルダーを作成する
	BehaviorTreeBuilder builder;

	return builder
       .RestartingSequence()

			// ロックオンしているターゲットがいるか
			.Condition([character](){return character->GetLockOnTarget() != nullptr;})
			.End()

			// 回避しているか
			.Condition([character]() {return !character->IsAvoid(); })
			.End()

			// 回避する
			.Action_(std::make_unique<Avoid>(character, Vector3(0.0f, 0.0f, -1.0f)))
			.End()
		.End()
		.Build();
}
