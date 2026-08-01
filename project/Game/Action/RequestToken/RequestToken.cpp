#include "RequestToken.h"
#include "Entity/Character/Character.h"

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
	isRequested_ = BattleDirector::GetInstance().RequestAttackToken(owner_, tokenType_);

	// 攻撃トークンを要求できたら、スロットを開放する
	if (isRequested_)BattleDirector::GetInstance().ReleaseSlot(owner_);
}