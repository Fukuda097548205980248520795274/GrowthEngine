#pragma once
#include "../CharacterState.h"

class Character;

class CharacterStateDash : public CharacterState
{
public:

	/// @brief コンストラクタ
	/// @param owner 
	CharacterStateDash(Character* owner, AnimationHandle hMotion)
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

	/// @brief ダッシュモーションのハンドル
	AnimationHandle hMotion_ = 0;

	// ダッシュ中かどうか
	bool isDash_ = false;
};

