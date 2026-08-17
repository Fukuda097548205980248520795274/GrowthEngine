#include "RequestToken.h"
#include "Entity/Character/Character.h"

/// @brief 実行
void RequestToken::Exec()
{
	// ブレークポイントのチェック
	BreakpointOnExec();

	// 基底クラスの実行
	Action::Exec();

	// 攻撃クールタイム中なら終了する
	if (owner_->GetAttackCooltime() > 0.0f)
	{
		Exit();
		return;
	}

	// 攻撃トークンを要求する
	if (BattleDirector::GetInstance().RequestAttackToken(owner_, tokenType_))
	{
		// スロットを解放する
		BattleDirector::GetInstance().ReleaseSlot(owner_);
	}
	else
	{
		// トークン要求に失敗した場合は終了する
		Exit();
		return;
	}

	// ここまで来たら成功
	Action::Update();
}

/// @brief 終了、中断
void RequestToken::Exit()
{
	BreakpointOnExit();

	// 終了する
	Action::Exit();
}

/// @brief リセット
void RequestToken::Reset()
{
	// ブレークポイントのチェック
	BreakpointOnReset();

	// 基底クラスのリセット
	Action::Reset();
}