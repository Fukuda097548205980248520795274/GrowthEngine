#pragma once
#include "../CharacterState.h"

class Attack;
class Character;

class CharacterStateAttack : public CharacterState
{
public:

	/// @brief コンストラクタ
	/// @param owner 
	CharacterStateAttack(Character* owner) : CharacterState(owner) {}

	/// @brief この状態に入るときに呼ばれる処理
	void Enter() override;

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt) override;

	/// @brief この状態からでるときに呼ばれる処理
	void Exit() override;

	/// @brief 攻撃を設定する
	/// @param attack 
	void SetAttack(Attack* attack) { currentAttack_ = attack; }

	/// @brief 攻撃を取得する
	/// @return 
	Attack* GetCurrentAttack() const { return currentAttack_; }


private:

	/// @brief 今の攻撃
	Attack* currentAttack_ = nullptr;
};

