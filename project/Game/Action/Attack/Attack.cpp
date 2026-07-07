#include "Attack.h"
#include "Entity/Character/Character.h"

/// @brief 実行
void Attack::Exec()
{
	// 攻撃ポインタを渡す
	owner_->SetCurrentAttack(this);

	// 基底クラスのExecを呼び出す
	Action::Exec();
}

/// @brief 中断・終了
void Attack::Exit()
{
	// もし現在の攻撃が自分自身であれば、攻撃ポインタをnullptrにする
	owner_->SetCurrentAttack(nullptr);

	Action::Exit();
}

/// @brief リセット
void Attack::Reset()
{
	// もし現在の攻撃が自分自身であれば、攻撃ポインタをnullptrにする
	owner_->SetCurrentAttack(nullptr);

	Action::Reset();
}

/// @brief 更新処理
void Attack::Update()
{
	// 基底クラスのUpdateを呼び出す
	Action::Update();
}

/// @brief 攻撃中かどうか
/// @return 
bool Attack::IsUse() const
{
	if (!IsExec()) return false;

	// 現在の攻撃が自分自身かどうか
	return this == owner_->GetCurrentAttack();
}

/// @brief 次の攻撃があるかどうか
/// @param inputType 
/// @return 
bool Attack::HasNextAttack(AttackInputType inputType) const
{
	if (inputType == AttackInputType::InputX) return nextInputXAttack_ != nullptr;
	if (inputType == AttackInputType::InputY) return nextInputYAttack_ != nullptr;
	if (inputType == AttackInputType::InputB) return nextInputBAttack_ != nullptr;
	return false;
}