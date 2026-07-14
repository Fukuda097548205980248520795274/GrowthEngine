#pragma once
#include "../CharacterState.h"

class Character;

class CharacterStateDamage : public CharacterState
{
public:

	/// @brief 怯みの種類
	enum class DamageReactionType
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
	CharacterStateDamage(Character* owner, AnimationHandle hFront, AnimationHandle hBack, AnimationHandle hLeft, AnimationHandle hRight, float maxDamageTime = 0.5f);

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

	/// @brief ダメージ時間の最大値を設定する
	/// @param maxDamageTime 
	void SetMaxDamageTime(float maxDamageTime) { maxDamageTime_ = maxDamageTime; }


private:

	/// @brief アクション
	DamageReactionType reaction_ = DamageReactionType::None;

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

