#pragma once
#include "../CharacterState.h"

class Character;

class CharacterStateParried : public CharacterState
{
public:

	/// @brief コンストラクタ
	/// @param owner 
	CharacterStateParried(Character* owner, AnimationHandle hMotion);

	/// @brief この状態に入るときに呼ばれる処理
	void Enter() override;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
	void Exit() override;


private:

	/// @brief ダメージ状態の最大時間
	float maxDamageTime_ = 1.0f;

	// ダメージ状態のタイマー
	float damageTimer_ = 0.0f;

	/// @brief アニメーションハンドル
	AnimationHandle hMotion_ = 0;
};

