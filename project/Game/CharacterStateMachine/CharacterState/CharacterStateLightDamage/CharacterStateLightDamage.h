#pragma once
#include "../CharacterState.h"

class Character;

class CharacterStateLightDamage : public CharacterState
{
public:

	/// @brief 軽い怯みの種類
	enum class LightDamageReaction
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
	CharacterStateLightDamage(Character* owner, AnimationHandle hFront, AnimationHandle hBack, AnimationHandle hLeft, AnimationHandle hRight);

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
	LightDamageReaction reaction_ = LightDamageReaction::None;

	/// @brief ダメージ状態の最大時間
	float maxDamageTime_ = 0.5f;

	// ダメージ状態のタイマー
	float damageTimer_ = 0.0f;

	/// @brief アニメーションハンドル
	AnimationHandle hFront_ = 0;
	AnimationHandle hBack_ = 0;
	AnimationHandle hLeft_ = 0;
	AnimationHandle hRight_ = 0;
};

