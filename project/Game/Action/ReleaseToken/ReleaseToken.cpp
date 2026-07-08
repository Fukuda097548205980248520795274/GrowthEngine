#include "ReleaseToken.h"
#include "Entity/Character/Character.h"
#include "BattleDirector/BattleDirector.h"

/// @brief 実行
void ReleaseToken::Exec()
{
	// ブレークポイントのチェック
	BreakpointOnExec();

	// 攻撃トークンを返却する
	BattleDirector::GetInstance().ReleaseAttackToken(owner_);

	// 攻撃クールタイムをリセットする
	owner_->SetAttackCooltime(0.5f);
}

/// @brief 終了、中断
void ReleaseToken::Exit()
{
	// ブレークポイントのチェック
	BreakpointOnExit();

	// 攻撃トークンを返却する
	BattleDirector::GetInstance().ReleaseAttackToken(owner_);
}