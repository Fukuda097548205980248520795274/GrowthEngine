#include "CharacterStateGuard.h"
#include <numbers>
#include "Entity/Character/Player/Player.h"
#include "Entity/Character/NPC/NPC.h"

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateGuard::Enter()
{
	// ツリーのリクエストを行う
	TreeRequest();

	// 防御のリアクション中かどうかをリセットする
	isGuardReaction_ = false;
	guardReactionTimer_ = 0.0f;

	// 防御しているタイマーをリセットする
	guardActiveTimer_ = 0.0f;

	// 移動を停止する
	owner_->MoveStop();

	// 防御モーションを再生する
	owner_->SetAnimation(hGuard_, true, true);
}

/// @brief 更新処理
/// @param dt 
void CharacterStateGuard::Update(float dt)
{
	/// ツリーのリクエストを行う
	HandleBehaviorTreeNotSet();

	// 移動コンポーネントとワールドトランスフォームを取得する
	auto movement = owner_->GetMovement();
	WorldTransform3D* worldTransform = owner_->GetWorldTransform();

	// 防御しているタイマーを更新する
	guardActiveTimer_ += dt;

	// ガードリアクション中は、攻撃してきた相手の方向を向くようにする
	if (isGuardReaction_)
	{
		float diff = movement->GetTargetRotationY() - worldTransform->rotate_.y;
		const float kPi = std::numbers::pi_v<float>;

		// 角度の正規化
		while (diff > kPi) diff -= 2.0f * kPi;
		while (diff < -kPi) diff += 2.0f * kPi;

		worldTransform->rotate_.y += diff * movement->GetRotationSpeed() * dt;


		// 防御のリアクションの経過時間を更新する
		guardReactionTimer_ += dt;
		if (guardReactionTimer_ >= kGuardReactionDuration)
		{
			isGuardReaction_ = false;

			// 防御モーションを再生する
			owner_->SetAnimation(hGuard_, true, true);
		}
	}

	// プレイヤーの場合は、防御入力が解除されたら状態を解除する
	if (owner_->IsPlayer())
	{
		Player* player = static_cast<Player*>(owner_);
		auto inputController = player->GetInputController();

		if (!inputController->IsGuardRequested())
		{
			// 防御入力が解除された場合は、状態を解除する
			auto stateMachine = owner_->GetStateMachine();
			stateMachine->ChangeState("None");
			return;
		}
	}
	else
	{
		if (guardActiveTimer_ >= guardDuration_)
		{
			// NPCの場合は、防御時間が経過したら状態を解除する
			auto stateMachine = owner_->GetStateMachine();
			stateMachine->ChangeState("None");
			return;
		}
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateGuard::Exit()
{
	// 防御のリアクション中かどうかをリセットする
	isGuardReaction_ = false;
	guardReactionTimer_ = 0.0f;

	// 防御しているタイマーをリセットする
	guardActiveTimer_ = 0.0f;

	// NPCの場合は、受け流しと弾きのフラグをリセットする
	if (!owner_->IsPlayer())
	{
		owner_->SetCanDeflect(false);
		owner_->SetCanRepel(false);
	}
}

/// @brief ガードがヒットしたときの処理
void CharacterStateGuard::HitGuard()
{
	// リアクションを開始する
	isGuardReaction_ = true;
	guardReactionTimer_ = 0.0f;

	// ガードヒットモーションを再生する
	owner_->SetAnimation(hHitGuard_, true, false);
}

/// @brief パリィの種類を設定する
/// @param parryType 
void CharacterStateGuard::SetParryType(ParryType parryType)
{
	owner_->SetCanDeflect(false);
	owner_->SetCanRepel(false);

	// パリィの種類に応じて、受け流しと弾きのフラグを設定する
	switch (parryType)
	{
	case ParryType::Deflect:
		owner_->SetCanDeflect(true);
		break;

	case  ParryType::Repel:
		owner_->SetCanRepel(true);
		break;
	}
}