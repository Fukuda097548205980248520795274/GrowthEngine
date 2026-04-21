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

/// @brief 更新処理
void Attack::Update()
{
	// 基底クラスのUpdateを呼び出す
	Action::Update();
}