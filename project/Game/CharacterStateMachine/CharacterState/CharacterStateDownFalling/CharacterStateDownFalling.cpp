#include "CharacterStateDownFalling.h"
#include "Entity/Character/Character.h"
#include "../CharacterStateDownLying/CharacterStateDownLying.h"
#include "SoundManager/SoundManager.h"

/// @brief コンストラクタ
	/// @param owner 
CharacterStateDownFalling::CharacterStateDownFalling(Character* owner, AnimationHandle hFront, AnimationHandle hBack, AnimationHandle hLeft, AnimationHandle hRight)
	: CharacterState(owner), hFront_(hFront), hBack_(hBack), hLeft_(hLeft), hRight_(hRight)
{
	// サウンドマネージャーを取得する
	soundManager_ = SoundManager::GetInstance();
}

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateDownFalling::Enter()
{
	// ダメージタイマーをリセットする
	damageTimer_ = maxDamageTime_;
}

/// @brief 更新処理
/// @param dt 
void CharacterStateDownFalling::Update(float dt)
{
	// ダメージタイマーを更新する
	damageTimer_ -= dt;

	// ダメージタイマーが0以下になったら、ダウン状態へ移行する
	if (damageTimer_ <= 0.0f)
	{
		auto stateMachine = owner_->GetStateMachine();
		if (auto nextState = static_cast<CharacterStateDownLying*>(stateMachine->GetState("DownLying")))
		{
			// ダメージリアクションの方向を設定する
			if (reaction_ == DownFallingDamageReaction::Front || reaction_ == DownFallingDamageReaction::Right)
			{
				nextState->DamageReaction(CharacterStateDownLying::DownLyingDamageReaction::Front);
			}
			else if (reaction_ == DownFallingDamageReaction::Back || reaction_ == DownFallingDamageReaction::Left)
			{
				nextState->DamageReaction(CharacterStateDownLying::DownLyingDamageReaction::Back);
			}
		}

		// 状態をダウン状態に変更する
		stateMachine->ChangeState("DownLying");

		// ダウン中着地のSEを再生する
		soundManager_->SeDownLanding();

		return;
	}
}

/// @brief この状態からでるときに呼ばれる処理
void CharacterStateDownFalling::Exit()
{
	// ダメージリアクションをリセットする
	reaction_ = DownFallingDamageReaction::None;
}

/// @brief ダメージリアクションを起こす
/// @param hitPosition 
/// @param attacker 
void CharacterStateDownFalling::DamageReaction(const Vector3& hitPosition)
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
		reaction_ = DownFallingDamageReaction::Left;
		owner_->SetAnimation(hLeft_, true, false);
	}
	else if (localX > 0.0f && localZ > 0.0f)
	{
		reaction_ = DownFallingDamageReaction::Right;
		owner_->SetAnimation(hRight_, true, false);
	}
	else if (localX < 0.0f && localZ < 0.0f)
	{
		reaction_ = DownFallingDamageReaction::Back;
		owner_->SetAnimation(hBack_, true, false);
	}
	else
	{
		reaction_ = DownFallingDamageReaction::Front;
		owner_->SetAnimation(hFront_, true, false);
	}
}