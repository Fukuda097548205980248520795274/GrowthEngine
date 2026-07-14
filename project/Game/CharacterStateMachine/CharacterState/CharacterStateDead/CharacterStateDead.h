#pragma once
#include "../CharacterState.h"

class Character;

class CharacterStateDead : public CharacterState
{
public:

	/// @brief コンストラクタ
	/// @param owner 
	CharacterStateDead(Character* owner, AnimationHandle hMotion)
		: CharacterState(owner), hMotion_(hMotion) {
	}

	/// @brief この状態に入るときに呼ばれる処理
	void Enter() override;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
	void Exit() override;


private:

	// 死亡状態のタイマー
	float deadTimer_ = 0.0f;

	// 死亡状態の時間
	float deadTime_ = 3.0f;

	/// @brief 死亡モーションのハンドル
	AnimationHandle hMotion_ = 0;
};

