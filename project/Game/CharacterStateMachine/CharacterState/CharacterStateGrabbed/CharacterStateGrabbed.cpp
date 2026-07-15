#include "CharacterStateGrabbed.h"
#include "Entity/Character/NPC/NPC.h"
#include "Entity/Character/Player/Player.h"
#include <numbers>

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateGrabbed::Enter()
{
	// ビヘイビアツリーのリクエストを行う
	BehaviorTreeRequest();

	// タイマーをリセットする
	grabbedTimer_ = 0.0f;
}

/// @brief 更新処理
/// @param dt 
void CharacterStateGrabbed::Update(float dt)
{
	// 掴んでいる相手がいない場合は、掴まれ状態を解除する
	if (!grabber_)
	{
		owner_->GetStateMachine()->ChangeState("None");
		return;
	}

	if (owner_->GetCharacterTag() == CharacterTag::Player)
	{
		// プレイヤーの場合は、掴まれ解き入力を受け付ける
		Player* player = static_cast<Player*>(owner_);
		auto inputController_ = player->GetInputController();

		bool isStruggleInput = false;
		if (inputController_->IsEscapeMashRequested()) isStruggleInput = true;

		if (isStruggleInput)
		{
			grabbedTimer_ += 0.2f;
		}
	}
	else
	{
		// NPCの場合は、掴まれタイマーを更新する
		NPC* npc = static_cast<NPC*>(owner_);
		grabbedTimer_ += dt;
	}

	// 掴まれタイマーが過ぎたら、掴まれ状態を解除する
	if (grabbedTimer_ >= 3.0f)
	{
		Vector3 ownerPosition = owner_->GetWorldPosition();
		WorldTransform3D* ownerTransform = owner_->GetWorldTransform();

		if (grabber_)
		{
			// 振りほどかれた際の怯みを入れる
			grabber_->OnDamage(0, DamageReaction::LightStagger, 0.1f, Vector3(0.0f, 0.0f, -1.0f), ownerPosition);

			// 掴んでいる相手から離れる
			ownerTransform->rotate_ = Vector3(0.0f, grabber_->GetWorldTransform()->rotate_.y + std::numbers::pi_v<float>, 0.0f);
			ownerTransform->translate_.y = grabber_->GetWorldPosition().y;

			// 掴んでいる相手から離れる
			grabber_->SetGrabTarget(nullptr);
			grabber_ = nullptr;
		}

		// 掴まれタイマーをリセットする
		grabbedTimer_ = 0.0f;

		// ダメージリアクションを解除する
		owner_->GetStateMachine()->ChangeState("None");
		return;
	}

	// 使いでいるはずの相手が掴んでいない場合は、掴まれ状態を解除する
	if (!grabber_->GetGrabTarget())
	{
		owner_->GetStateMachine()->ChangeState("None");
		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateGrabbed::Exit()
{
	// 掴んでいる相手から離れる
	grabber_ = nullptr;
}