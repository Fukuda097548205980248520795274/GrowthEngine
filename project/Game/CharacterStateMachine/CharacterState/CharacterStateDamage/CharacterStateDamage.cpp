#include "CharacterStateDamage.h"
#include "Entity/Character/Character.h"

/// @brief コンストラクタ
	/// @param owner 
CharacterStateDamage::CharacterStateDamage(Character* owner, AnimationHandle hFront, AnimationHandle hBack, AnimationHandle hLeft, AnimationHandle hRight, float maxDamageTime)
	: CharacterState(owner), hFront_(hFront), hBack_(hBack), hLeft_(hLeft), hRight_(hRight), maxDamageTime_(maxDamageTime)
{
}

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateDamage::Enter()
{
	// ビヘイビアツリーのリクエストを行う
	BehaviorTreeRequest();

	// プレイヤーとボス以外のキャラクター、または攻撃中でない場合は、ダメージタイマーをリセットする
	if ((!owner_->IsPlayer() && !owner_->IsBoss()) || !owner_->IsAttack())
	{
		// ダメージタイマーをリセットする
		damageTimer_ = maxDamageTime_;
	}
	else
	{
		damageTimer_ = 0.0f;
	}
}

/// @brief 更新処理
/// @param dt 
void CharacterStateDamage::Update(float dt)
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
void CharacterStateDamage::Exit()
{
	// ダメージリアクションをリセットする
	reaction_ = DamageReactionType::None;
}

/// @brief ダメージリアクションを起こす
/// @param hitPosition 
/// @param attacker 
void CharacterStateDamage::DamageReaction(const std::optional<Vector3>& hitPosition)
{
	// 既にタイマーが0以下の場合は、ここで終了する
	if (damageTimer_ <= 0.0f)
	{
		owner_->GetStateMachine()->ChangeState("None");
		return;
	}


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
		if (localX < 0.0f && localZ > 0.0f)
		{
			reaction_ = DamageReactionType::Left;
			owner_->SetAnimation(hLeft_, true, false);
		}
		else if (localX > 0.0f && localZ > 0.0f)
		{
			reaction_ = DamageReactionType::Right;
			owner_->SetAnimation(hRight_, true, false);
		}
		else if (localX < 0.0f && localZ < 0.0f)
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
		// hitPositionがない場合は、正面のリアクションをデフォルトとして設定する
		reaction_ = DamageReactionType::Front;
		owner_->SetAnimation(hFront_, true, false);
	}
}