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

bool Move::IsUse() const
{
	// 実行していない場合は使用していないとみなす
	if (!IsExec())return false;

	return this == owner_->GetCurrentMove();
}