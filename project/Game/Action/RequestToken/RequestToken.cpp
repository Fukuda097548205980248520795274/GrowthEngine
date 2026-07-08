#include "RequestToken.h"
#include "Entity/Character/Character.h"
#include "BattleDirector/BattleDirector.h"

/// @brief 実行
void RequestToken::Exec()
{
	// ブレークポイントのチェック
	BreakpointOnExec();

	// トークン要求フラグをリセット
	isRequested_ = false;

	// 攻撃のクールタイムが残っている場合は、トークンを要求せずに終了する
	if (owner_->GetAttackCooltime() > 0.0f)
		return;

	// 攻撃トークンを要求する
	isRequested_ = BattleDirector::GetInstance().RequestAttackToken(owner_);
}