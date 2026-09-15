#pragma once
#include "../CharacterState.h"

class CharacterStateNone : public CharacterState
{
public:

	/// @brief コンストラクタ
    /// @param owner 
    CharacterStateNone(Character* owner, AnimationHandle hStandMotion, AnimationHandle hStanceMotion, AnimationHandle hWalkMotion ) : 
		CharacterState(owner), hStandMotion_(hStandMotion), hStanceMotion_(hStanceMotion), hWalkMotion_(hWalkMotion) {}

	/// @brief この状態に入るときに呼ばれる処理
    void Enter() override;

	/// @brief 更新処理
    /// @param dt 
    void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
    void Exit() override {}


private:

	/// @brief 立ちモーション
    AnimationHandle hStandMotion_ = 0;

	/// @brief 構えモーション
	AnimationHandle hStanceMotion_ = 0;

	/// @brief 歩きモーション
	AnimationHandle hWalkMotion_ = 0;
};

