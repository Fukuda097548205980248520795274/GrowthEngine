#pragma once
#include "../CharacterState.h"

class CharacterStateNone : public CharacterState
{
public:

	/// @brief コンストラクタ
    /// @param owner 
    CharacterStateNone(Character* owner) : CharacterState(owner) {}

	/// @brief この状態に入るときに呼ばれる処理
    void Enter() override;

	/// @brief 更新処理
    /// @param dt 
    void Update(float dt) override 
    {
        /// ツリーのリクエストを行う
        HandleBehaviorTreeNotSet();
    }

	/// @brief この状態からでるときに呼ばれる処理
    void Exit() override {}
};

