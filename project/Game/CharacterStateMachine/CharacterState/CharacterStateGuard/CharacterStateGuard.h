#pragma once
#include "../CharacterState.h"
#include "Action/Defense/Defense.h"

class Character;

class CharacterStateGuard : public CharacterState
{
public:

	/// @brief コンストラクタ
	/// @param owner 
	CharacterStateGuard(Character* owner, AnimationHandle hGuard, AnimationHandle hHitGuard)
		: CharacterState(owner), hGuard_(hGuard), hHitGuard_(hHitGuard) {
	}

	/// @brief この状態に入るときに呼ばれる処理
	void Enter() override;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
	void Exit() override;

	/// @brief ジャストガードが可能かどうか
	/// @return 
	bool CanJustGuard() const { return guardActiveTimer_ <= kJustGuardTime; }

	/// @brief ガードがヒットしたときの処理
	void HitGuard();

	/// @brief ガードする時間を設定する
	/// @param duration 
	void SetGuardDuration(float duration) { guardDuration_ = duration; }

	/// @brief パリィの種類を設定する
	/// @param parryType 
	void SetParryType(ParryType parryType);


private:

	/// @brief 防御のリアクション中かどうか
	bool isGuardReaction_ = false;

	/// @brief 防御のリアクションの経過時間
	float guardReactionTimer_ = 0.0f;

	/// @brief 防御のリアクションの時間
	const float kGuardReactionDuration = 0.3f;


	// ガードしてからの経過時間
	float guardActiveTimer_ = 0.0f;

	// ジャストガード（受け流し）の受付時間
	const float kJustGuardTime = 0.2f;

	// ガードする時間
	float guardDuration_ = 0.0f;


	// ガードモーション
	AnimationHandle hGuard_ = 0;

	// ガード成功モーション
	AnimationHandle hHitGuard_ = 0;
};

