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
	// ツリーのリクエストを行う
	TreeRequest();

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
			if (reaction_ == DamageReactionType::Front || reaction_ == DamageReactionType::Right)
			{
				nextState->DamageReaction(CharacterStateDownLying::DamageReactionType::Front);
			}
			else if (reaction_ == DamageReactionType::Back || reaction_ == DamageReactionType::Left)
			{
				nextState->DamageReaction(CharacterStateDownLying::DamageReactionType::Back);
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
	reaction_ = DamageReactionType::None;
}

/// @brief ダメージリアクションを起こす
/// @param hitPosition 
/// @param attacker 
void CharacterStateDownFalling::DamageReaction(const std::optional<Vector3>& hitPosition)
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
		// ヒット位置が不明な場合は、正面のダメージリアクションを再生する
		reaction_ = DamageReactionType::Front;
		owner_->SetAnimation(hFront_, true, false);
	}
}