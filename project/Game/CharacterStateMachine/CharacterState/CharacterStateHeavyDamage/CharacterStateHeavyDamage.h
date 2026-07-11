#pragma once
#include "../CharacterState.h"

class SoundManager;
class EffectManager;
class Character;

class CharacterStateHeavyDamage : public CharacterState
{
public:

	/// @brief 軽い怯みの種類
	enum class HeavyDamageReaction
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
	CharacterStateHeavyDamage(Character* owner, AnimationHandle hFront, AnimationHandle hBack, AnimationHandle hLeft, AnimationHandle hRight);

	/// @brief この状態に入るときに呼ばれる処理
	void Enter() override;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
	void Exit() override;

	/// @brief ダメージリアクションを起こす
	/// @param hitPosition 
	void DamageReaction(const std::optional<Vector3>& hitPosition);


private:

	/// @brief アクション
	HeavyDamageReaction reaction_ = HeavyDamageReaction::None;

	/// @brief ダメージ状態の最大時間
	float maxDamageTime_ = 1.5f;

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

	/// @brief エフェクトマネージャー
	EffectManager* effectManager_ = nullptr;
};

