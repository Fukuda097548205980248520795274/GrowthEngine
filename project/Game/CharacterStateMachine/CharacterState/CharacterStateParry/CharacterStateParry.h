#pragma once
#include "../CharacterState.h"

class Character;

class CharacterStateParry : public CharacterState
{
public:

	/// @brief コンストラクタ
	/// @param owner 
	CharacterStateParry(Character* owner, AnimationHandle hMotion);

	/// @brief この状態に入るときに呼ばれる処理
	void Enter() override;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
	void Exit() override;


private:

	/// @brief アクションの最大時間
	float maxActionTime_ = 0.2f;

	// アクションのタイマー
	float actionTimer_ = 0.0f;

	/// @brief アニメーションハンドル
	AnimationHandle hMotion_ = 0;
};

