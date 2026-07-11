#pragma once
#include "../CharacterState.h"

class Character;
class SoundManager;

class CharacterStateDownFalling : public CharacterState
{
public:

	/// @brief 軽い怯みの種類
	enum class DownFallingDamageReaction
	{
		None,
		Front,
		Back,
		Left,
		Right
	};


public:

	/// @brief コンストラクタ
	/// @param owner 
	CharacterStateDownFalling(Character* owner, AnimationHandle hFront, AnimationHandle hBack, AnimationHandle hLeft, AnimationHandle hRight);

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
	void DamageReaction(const Vector3& hitPosition);


private:

	/// @brief アクション
	DownFallingDamageReaction reaction_ = DownFallingDamageReaction::None;

	/// @brief ダメージ状態の最大時間
	float maxDamageTime_ = 1.0f;

	// ダメージ状態のタイマー
	float damageTimer_ = 0.0f;

	/// @brief アニメーションハンドル
	AnimationHandle hFront_ = 0;
	AnimationHandle hBack_ = 0;
	AnimationHandle hLeft_ = 0;
	AnimationHandle hRight_ = 0;


private:

	/// @brief サウンドマネージャー
	SoundManager* soundManager_ = nullptr;
};

