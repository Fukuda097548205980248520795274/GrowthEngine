#include "CharacterStateBlownFalling.h"
#include "Entity/Character/Character.h"
#include "../CharacterStateDownLying/CharacterStateDownLying.h" 
#include "SoundManager/SoundManager.h"

/// @brief コンストラクタ
	/// @param owner 
CharacterStateBlownFalling::CharacterStateBlownFalling(Character* owner, AnimationHandle hFront, AnimationHandle hBack)
	: CharacterState(owner), hFront_(hFront), hBack_(hBack)
{
	// サウンドマネージャーのインスタンスを取得する
	soundManager_ = SoundManager::GetInstance();
}

/// @brief この状態に入るときに呼ばれる処理
void CharacterStateBlownFalling::Enter()
{

}

/// @brief 更新処理
/// @param dt 
void CharacterStateBlownFalling::Update(float dt)
{
	// キャラクターの移動コンポーネントを取得する
	auto movement = owner_->GetMovement();
	bool isGrounded = movement ? movement->IsGrounded() : false;

	// 着地した場合は、ダウン状態へ移行する
	if (isGrounded)
	{
		auto stateMachine = owner_->GetStateMachine();
		if (auto nextState = static_cast<CharacterStateDownLying*>(stateMachine->GetState("DownLying")))
		{
			// ダメージリアクションの方向を設定する
			if (reaction_ == BlownFallingDamageReaction::Front)
			{
				nextState->DamageReaction(CharacterStateDownLying::DownLyingDamageReaction::Front);
			}
			else if (reaction_ == BlownFallingDamageReaction::Back)
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
void CharacterStateBlownFalling::Exit()
{
	// ダメージリアクションをリセットする
	reaction_ = BlownFallingDamageReaction::None;
}

/// @brief ダメージリアクションを起こす
/// @param hitPosition 
/// @param attacker 
void CharacterStateBlownFalling::DamageReaction(BlownFallingDamageReaction reaction)
{
	// 攻撃した側がプレイヤーの場合は、スローモーションを開始する
	reaction_ = reaction;

	// プレイヤーがダメージを受けた場合は、スローモーションを開始する
	if (reaction_ == BlownFallingDamageReaction::Back)
	{
		owner_->SetAnimation(hBack_, true, false);
	}
	else if (reaction_ == BlownFallingDamageReaction::Front)
	{
		owner_->SetAnimation(hFront_, true, false);
	}
}

/// @brief ダメージリアクションを起こす
void CharacterStateBlownFalling::DamageReaction()
{
	// 何も設定されていない場合は、前方向のリアクションを設定する
	if (reaction_ == BlownFallingDamageReaction::None)
		reaction_ = BlownFallingDamageReaction::Front;

	// プレイヤーがダメージを受けた場合は、スローモーションを開始する
	if (reaction_ == BlownFallingDamageReaction::Back)
	{
		owner_->SetAnimation(hBack_, true, false);
	}
	else if (reaction_ == BlownFallingDamageReaction::Front)
	{
		owner_->SetAnimation(hFront_, true, false);
	}
}