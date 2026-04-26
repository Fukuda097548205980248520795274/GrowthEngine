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

	// 一段目の攻撃
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
	attack1Data.staggerTime = 0.3f;
	attack1Data.knockback = 0.0f;

	// 二段目の攻撃
	CombAttackInitData attack2Data;
	attack2Data.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, 1);
	attack2Data.attackTime = 0.5f;
	attack2Data.moveSpeed = 9.0f;
	attack2Data.moveStartTime = 0.01f;
	attack2Data.moveEndTime = 0.07f;
	attack2Data.cancelStartTime = 0.2f;
	attack2Data.cancelEndTime = 0.5f;
	attack2Data.partName = "LeftHand";
	attack2Data.hitboxStartTime = 0.1f;
	attack2Data.hitboxEndTime = 0.4f;
	attack2Data.damage = 1;
	attack2Data.staggerTime = 0.3f;
	attack2Data.knockback = 0.0f;

	// 三段目の攻撃
	CombAttackInitData attack3Data;
	attack3Data.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, 2);
	attack3Data.attackTime = 0.5f;
	attack3Data.moveSpeed = 9.0f;
	attack3Data.moveStartTime = 0.01f;
	attack3Data.moveEndTime = 0.07f;
	attack3Data.cancelStartTime = 0.2f;
	attack3Data.cancelEndTime = 0.5f;
	attack3Data.partName = "RightHand";
	attack3Data.hitboxStartTime = 0.1f;
	attack3Data.hitboxEndTime = 0.4f;
	attack3Data.damage = 1;
	attack3Data.staggerTime = 0.3f;
	attack3Data.knockback = 0.0f;

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

					// ① ターゲットとの距離が近いか（攻撃範囲内か）を判定する
					.Condition([character]() {
					Character* target = character->GetLockOnTarget();
					if (!target) return false;

					Vector3 toTarget = target->GetPosition() - character->GetPosition();
					// 例：距離が2.5メートル以内なら攻撃可能
					return toTarget.Length() <= 2.5f;}).End()

					// ② 弱攻撃 1段目
					.ComboAttack_(std::make_unique<ComboAttack>(character, attack1Data)).End()
					// ③ 弱攻撃 2段目（1段目のキャンセル時間になったらここに進む）
					.ComboAttack_(std::make_unique<ComboAttack>(character, attack2Data)).End()
					// ④ 強攻撃 3段目（フィニッシュ）
					.ComboAttack_(std::make_unique<ComboAttack>(character, attack3Data)).End()

				.End() // コンボシーケンス終了


				// ----------------------------------------------------
				// [ルートB] ルートAの条件（距離2.5m以内）を満たさなかった場合の移動
				// ----------------------------------------------------
				.PersistentSequence()
				// ターゲットに2.0メートルまで近づく
					.Action_(std::make_unique<ApproachTargetMove>(character, 2.0f, 3.0f)).End()
				.End() // 移動シーケンス終了

			.End() // Selector終了

		.Build();
}
