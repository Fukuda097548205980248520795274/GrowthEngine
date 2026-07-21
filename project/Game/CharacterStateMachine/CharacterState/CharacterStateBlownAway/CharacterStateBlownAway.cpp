#include "CharacterStateBlownAway.h"
#include "Entity/Character/Character.h"
#include "../CharacterStateBlownFalling/CharacterStateBlownFalling.h"

/// @brief コンストラクタ
	/// @param owner 
CharacterStateBlownAway::CharacterStateBlownAway(Character* owner, AnimationHandle hFront, AnimationHandle hBack)
	: CharacterState(owner), hFront_(hFront), hBack_(hBack)
{
}

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateBlownAway::Enter()
{
	// ツリーのリクエストを行う
	TreeRequest();
}

/// @brief 更新処理
/// @param dt 
void CharacterStateBlownAway::Update(float dt)
{
	/// ツリーのリクエストを行う
	HandleBehaviorTreeNotSet();

	// キャラクターの移動コンポーネントを取得する
	auto movement = owner_->GetMovement();
	Vector3 knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);
	float velocityY = 0.0f;

	// ノックバックの方向を取得する
	if (movement)
	{
		knockbackDirection = movement->GetKnockbackVelocity();
		velocityY = movement->GetVelocityY();
	}

	// キャラクターが落ち始めたら、ダウン落下状態へ移行する
	if (knockbackDirection.y + velocityY <= 0.0f)
	{
		auto stateMachine = owner_->GetStateMachine();
		if (auto nextState = static_cast<CharacterStateBlownFalling*>(stateMachine->GetState("BlownFalling")))
		{
			// ダメージリアクションを設定する
			if (reaction_ == DamageReactionType::Front)
			{
				nextState->DamageReaction(CharacterStateBlownFalling::DamageReactionType::Front);
			}
			else if (reaction_ == DamageReactionType::Back)
			{
				nextState->DamageReaction(CharacterStateBlownFalling::DamageReactionType::Back);
			}
		}

		// 状態をダウン落下状態に変更する
		stateMachine->ChangeState("BlownFalling");

		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateBlownAway::Exit()
{
	// ダメージリアクションをリセットする
	reaction_ = DamageReactionType::None;
}

/// @brief ダメージリアクションを起こす
/// @param hitPosition 
/// @param attacker 
void CharacterStateBlownAway::DamageReaction(const std::optional<Vector3>& hitPosition)
{
	/*----------------------
		向きによる種類分け
	----------------------*/

	if (hitPosition)
	{
		Vector3 hitPos = hitPosition.value();

		Vector2 ownerDirectionXZ = Vector2(owner_->GetDirection().x, owner_->GetDirection().z).Normalize();
		Vector2 hitDirectionXZ = Vector2(hitPos.x - owner_->GetWorldPosition().x, hitPos.z - owner_->GetWorldPosition().z).Normalize();

		// 右方向ベクトルを計算する
		Vector2 rightDirectionXZ = Vector2(ownerDirectionXZ.y, -ownerDirectionXZ.x);

		// XZ平面でのローカル座標を計算する
		float localX = -Dot(hitDirectionXZ, rightDirectionXZ);
		float localZ = -Dot(hitDirectionXZ, ownerDirectionXZ);

		// ダメージリアクションの方向を判定する
		if (localZ < 0.0f)
		{
			reaction_ = DamageReactionType::Back;
			owner_->SetAnimation(hBack_, true, false);
		}
		else
		{
			reaction_ = DamageReactionType::Front;
			owner_->SetAnimation(hFront_, true, false);
		}
	}
	else
	{
		// ヒット位置が不明な場合は、正面のダメージリアクションを再生する
		reaction_ = DamageReactionType::Front;
		owner_->SetAnimation(hFront_, true, false);
	}
}