#pragma once
#include "../CharacterState.h"

class Character;

class CharacterStateDownLying : public CharacterState
{
public:

	/// @brief 軽い怯みの種類
	enum class DownLyingDamageReaction
	{
		None,
		Front,
		Back,
	};


public:

	/// @brief コンストラクタ
	/// @param owner 
	CharacterStateDownLying(Character* owner, AnimationHandle hFront, AnimationHandle hBack);

	/// @brief この状態に入るときに呼ばれる処理
	void Enter() override;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
	void Exit() override;

	/// @brief ダメージリアクションを起こす
	/// @param hitPosition 
	/// @param attacker 
	void DamageReaction(DownLyingDamageReaction reaction);


private:

	/// @brief アクション
	DownLyingDamageReaction reaction_ = DownLyingDamageReaction::None;

	/// @brief ダメージ状態の最大時間
	float maxDamageTime_ = 2.0f;

	// ダメージ状態のタイマー
	float damageTimer_ = 0.0f;

	/// @brief アニメーションハンドル
	AnimationHandle hFront_ = 0;
	AnimationHandle hBack_ = 0;
};

