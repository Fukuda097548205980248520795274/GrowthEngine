#pragma once
#include "../CharacterState.h"

class Character;

class CharacterStateGrabbing : public CharacterState
{
public:

	/// @brief コンストラクタ
	/// @param owner 
	CharacterStateGrabbing(Character* owner) : CharacterState(owner) {}

	/// @brief この状態に入るときに呼ばれる処理
	void Enter() override;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
	void Exit() override;

	/// @brief 自分が掴んでいるターゲットを設定する
	/// @param grabTarget 
	void SetGrabTarget(Character* grabTarget) { grabTarget_ = grabTarget; }

	/// @brief 自分が掴んでいるターゲットを取得する
	/// @param grabTarget 
	Character* GetGrabTarget()const { return grabTarget_; }


private:

	/// @brief 自分をつかんでいる相手
	Character* grabTarget_ = nullptr;
};

