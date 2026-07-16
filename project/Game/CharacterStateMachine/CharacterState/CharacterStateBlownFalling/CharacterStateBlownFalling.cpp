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
	// ツリーのリクエストを行う
	TreeRequest();
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
			if (reaction_ == DamageReactionType::Front)
			{
				nextState->DamageReaction(CharacterStateDownLying::DamageReactionType::Front);
			}
			else if (reaction_ == DamageReactionType::Back)
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
void CharacterStateBlownFalling::Exit()
{
	// ダメージリアクションをリセットする
	reaction_ = DamageReactionType::None;
}

/// @brief ダメージリアクションを起こす
/// @param hitPosition 
/// @param attacker 
void CharacterStateBlownFalling::DamageReaction(DamageReactionType reaction)
{
	// 攻撃した側がプレイヤーの場合は、スローモーションを開始する
	reaction_ = reaction;

	// プレイヤーがダメージを受けた場合は、スローモーションを開始する
	if (reaction_ == DamageReactionType::Back)
	{
		owner_->SetAnimation(hBack_, true, false);
	}
	else if (reaction_ == DamageReactionType::Front)
	{
		owner_->SetAnimation(hFront_, true, false);
	}
}

/// @brief ダメージリアクションを起こす
void CharacterStateBlownFalling::DamageReaction()
{
	// 何も設定されていない場合は、前方向のリアクションを設定する
	if (reaction_ == DamageReactionType::None)
		reaction_ = DamageReactionType::Front;

	// プレイヤーがダメージを受けた場合は、スローモーションを開始する
	if (reaction_ == DamageReactionType::Back)
	{
		owner_->SetAnimation(hBack_, true, false);
	}
	else if (reaction_ == DamageReactionType::Front)
	{
		owner_->SetAnimation(hFront_, true, false);
	}
}