#include "Move.h"
#include "Entity/Character/Character.h"

/// @brief 実行
void Move::Exec()
{
	// 移動ポインタを渡す
	owner_->SetCurrentMove(this);

	// 基底クラスのExecを呼び出す
	Action::Exec();
}

/// @brief 更新処理
void Move::Update()
{
	// 基底クラスのUpdateを呼び出す
	Action::Update();
}