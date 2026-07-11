#include "CharacterStateHeavyDamage.h"
#include "Entity/Character/Character.h"

/// @brief コンストラクタ
	/// @param owner 
CharacterStateHeavyDamage::CharacterStateHeavyDamage(Character* owner, AnimationHandle hFront, AnimationHandle hBack, AnimationHandle hLeft, AnimationHandle hRight)
	: CharacterState(owner), hFront_(hFront), hBack_(hBack), hLeft_(hLeft), hRight_(hRight)
{
	
}

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateHeavyDamage::Enter()
{
	// ダメージタイマーをリセットする
	damageTimer_ = maxDamageTime_;
}

/// @brief 更新処理
/// @param dt 
void CharacterStateHeavyDamage::Update(float dt)
{
	// ダメージタイマーを更新する
	damageTimer_ -= dt;

	// ダメージタイマーが0以下になったら、通常状態へ移行する
	if (damageTimer_ <= 0.0f)
	{
		owner_->GetStateMachine()->ChangeState("None");
		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateHeavyDamage::Exit()
{
	// ダメージリアクションをリセットする
	reaction_ = HeavyDamageReaction::None;
}

/// @brief ダメージリアクションを起こす
/// @param hitPosition 
/// @param attacker 
void CharacterStateHeavyDamage::DamageReaction(const Vector3& hitPosition)
{
	/*----------------------
		向きによる種類分け
	----------------------*/

	Vector2 ownerDirectionXZ = Vector2(owner_->GetDirection().x, owner_->GetDirection().z).Normalize();
	Vector2 hitDirectionXZ = Vector2(hitPosition.x - owner_->GetWorldPosition().x, hitPosition.z - owner_->GetWorldPosition().z).Normalize();

	// 右方向ベクトルを計算する
	Vector2 rightDirectionXZ = Vector2(ownerDirectionXZ.y, -ownerDirectionXZ.x);

	// XZ平面でのローカル座標を計算する
	float localX = -Dot(hitDirectionXZ, rightDirectionXZ);
	float localZ = -Dot(hitDirectionXZ, ownerDirectionXZ);

	// ダメージリアクションの方向を判定する
	if (localX < 0.0f && localZ > 0.0f)
	{
		reaction_ = HeavyDamageReaction::Left;
		owner_->SetAnimation(hLeft_, true, false);
	}
	else if (localX > 0.0f && localZ > 0.0f)
	{
		reaction_ = HeavyDamageReaction::Right;
		owner_->SetAnimation(hRight_, true, false);
	}
	else if (localX < 0.0f && localZ < 0.0f)
	{
		reaction_ = HeavyDamageReaction::Back;
		owner_->SetAnimation(hBack_, true, false);
	}
	else
	{
		reaction_ = HeavyDamageReaction::Front;
		owner_->SetAnimation(hFront_, true, false);
	}
}