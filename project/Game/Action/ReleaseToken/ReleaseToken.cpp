#include "ReleaseToken.h"
#include "Entity/Character/Character.h"
#include "BattleDirector/BattleDirector.h"

/// @brief 実行
void ReleaseToken::Exec()
{
	// ブレークポイントのチェック
	BreakpointOnExec();

	// 基底クラスの実行
	Action::Exec();

	// 攻撃トークンを返却する
	BattleDirector::GetInstance().ReleaseAttackToken(owner_);

	// 攻撃クールタイムをリセットする
	owner_->SetAttackCooltime(0.5f);

	// ここまで来たら成功
	Action::Update();
}

/// @brief 終了、中断
void ReleaseToken::Exit()
{
	// ブレークポイントのチェック
	BreakpointOnExit();

	// 攻撃クールタイムをリセットする
	Action::Exit();
}

/// @brief リセット
void ReleaseToken::Reset()
{
	// ブレークポイントのチェック
	BreakpointOnReset();

	// 攻撃トークンを返却する
	BattleDirector::GetInstance().ReleaseAttackToken(owner_);

	// 基底クラスのリセット
	Action::Reset();
}