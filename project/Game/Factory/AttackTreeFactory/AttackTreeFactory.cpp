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
       // 繰り返し実行するシーケンス
        .RestartingSequence()

            // 1. そもそもターゲットがいるか？（いなければ失敗して最初に戻る）
            .Condition([character](){ 
                return character->GetLockOnTarget() != nullptr; 
            }).End()

            // 2. ターゲットに近づく
            // 第2引数(stopDistance): 2.0f (2メートルまで近づいたら次のノードへ)
            // 第3引数(moveSpeed): 3.0f (移動速度)
            .Action_(std::make_unique<ApproachTargetMove>(character, 2.0f, 3.0f)).End()

            // 3. 近づき終わったら、後ろへ回避する
            // ターゲット方向の逆ベクトル（後ろ）を指定
            .Action_(std::make_unique<Avoid>(character, Vector3(0.0f, 0.0f, -1.0f))).End()

        .End()
        .Build();
}
