#pragma once
#include "../CharacterState.h"

/// @brief キャラクターの構え状態
class CharacterStateStance : public CharacterState
{
public:

	/// @brief コンストラクタ
    /// @param character 
    /// @param hStanceMotion 
    CharacterStateStance(Character* character, AnimationHandle hStanceMotion)
        : CharacterState(character), hStanceMotion_(hStanceMotion) {
    }

	/// @brief この状態に入るときに呼ばれる処理
    void Enter() override;

	/// @brief 更新処理
    /// @param dt 
    void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
    void Exit() override;

private:

    /// @brief アニメーションハンドル
    AnimationHandle hStanceMotion_;
};

