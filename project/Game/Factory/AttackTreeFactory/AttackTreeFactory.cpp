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
	BehaviorTreeBuilder builder;

	// 1段目の攻撃
	CombAttackInitData attack1Data;
	attack1Data.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, 0);
	attack1Data.attackTime = 0.5f;
	attack1Data.moveSpeed = 9.0f;
	attack1Data.moveStartTime = 0.01f;
	attack1Data.moveEndTime = 0.07f;
	attack1Data.cancelStartTime = 0.2f;
	attack1Data.cancelEndTime = 0.5f;
	attack1Data.partName = "RightHand";
	attack1Data.hitboxStartTime = 0.1f;
	attack1Data.hitboxEndTime = 0.4f;
	attack1Data.damage = 1;
	attack1Data.damageReaction = DamageReaction::DownFalling;
	attack1Data.knockback = 0.1f;

	// 掴み攻撃
	GrabAttackInitData grabData;
	grabData.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, 0);
	grabData.attackTime = 1.0f;
	grabData.moveSpeed = 3.0f;
	grabData.moveStartTime = 0.1f;
	grabData.moveEndTime = 0.3f;
	grabData.grabPartName = "RightHand";
	grabData.hitboxStartTime = 0.15f;
	grabData.hitboxEndTime = 0.35f;
	grabData.grabTime = 3.0f;


	return builder
		// 常に状況を監視してルートを切り替えるためRestartingSequenceを使用
		.RestartingSequence()

			// 1. そもそもターゲットがいるか？
			.Condition([character]() { return character->GetLockOnTarget() != nullptr; }).End()

			// 2. 「攻撃圏内ならコンボ」または「攻撃圏外なら近づく」の分岐
			.RestartingSelector()

				// ----------------------------------------------------
				// [ルートA] 攻撃圏内にいる場合のコンボ攻撃シーケンス
				// ----------------------------------------------------
				.PersistentSequence()

					// ターゲットが攻撃圏内にいるか？（例：距離が2.5メートル以内）
					.Condition([character]() {
					Character* target = character->GetLockOnTarget();
					if (!target) return false;

					Vector3 toTarget = target->GetPosition() - character->GetPosition();
					// 例：距離が2.5メートル以内なら攻撃可能
					return toTarget.Length() <= 2.5f;}).End()

					// ターゲットを掴んでいるか？（掴んでいないとコンボ攻撃できない）
					.Condition([character]() {return !character->IsGrabbing(); }).End()

					// 掴み攻撃を実行する
					.GrabAttack_(std::make_unique<GrabAttack>(character, grabData)).End()

				.End() // コンボシーケンス終了


				// ----------------------------------------------------
				// [ルートB] ルートAの条件（距離2.5m以内）を満たさなかった場合の移動
				// ----------------------------------------------------
				.PersistentSequence()

					// ターゲットを掴んでいるか？（掴んでいないとコンボ攻撃できない）
					.Condition([character]() {return !character->IsGrabbing(); }).End()

					// ターゲットに2.0メートルまで近づく
					.Action_(std::make_unique<ApproachTargetMove>(character, 2.0f, 3.0f)).End()
				.End() // 移動シーケンス終了

			.End() // Selector終了

		.Build();
}
