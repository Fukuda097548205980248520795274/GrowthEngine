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

	// アニメーションハンドルの仮定義（実際のゲーム内のモーションIDに合わせてください）
	constexpr AnimationHandle kCombo1Anim = 1;
	constexpr AnimationHandle kCombo2Anim = 2;
	constexpr AnimationHandle kCombo3Anim = 3;

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
					return toTarget.Length() <= 5.0f;}).End()

					// ② 弱攻撃 1段目
					// (character, animation, アニメ時間, 移動速度, 移動開始, 移動終了, キャンセル開始, キャンセル終了)
					.ComboAttack_(std::make_unique<ComboAttack>(character, MotionManager::GetInstance()->GetMotion(MotionType::Attack, 0), 0.5f, 9.0f, 0.01f, 0.07f, 0.2f, 0.5f)).End()

					// ③ 弱攻撃 2段目（1段目のキャンセル時間になったらここに進む）
					.ComboAttack_(std::make_unique<ComboAttack>(character, MotionManager::GetInstance()->GetMotion(MotionType::Attack,1), 0.5f, 9.0f, 0.01f, 0.07f, 0.2f, 0.5f)).End()

					// ④ 強攻撃 3段目（フィニッシュ）
					.ComboAttack_(std::make_unique<ComboAttack>(character, MotionManager::GetInstance()->GetMotion(MotionType::Attack,2), 0.5f, 9.0f, 0.01f, 0.07f, 0.2f, 0.5f)).End()

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
