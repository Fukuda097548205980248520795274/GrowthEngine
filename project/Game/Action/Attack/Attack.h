#pragma once
#include "../Action.h"

/// @brief 攻撃入力の種類
enum class AttackInputType
{
	None,
	Light,
	Heavy
};

class Attack : public Action
{
public:

	/// @brief コンストラクタ
	/// @param character 
	Attack(Character* character) : Action(character) {}

	/// @brief 実行
	virtual void Exec() override;

	/// @brief 更新処理
	virtual void Update() override;

	/// @brief 次の攻撃があるかどうか
	/// @return 
	virtual bool HasNextAttack(AttackInputType inputType) const { return false; }

	/// @brief 攻撃中かどうか
	/// @return 
	bool IsUse() const override;
};

