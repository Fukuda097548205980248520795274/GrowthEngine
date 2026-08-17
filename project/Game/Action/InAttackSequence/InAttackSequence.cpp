#include "InAttackSequence.h"
#include "Entity/Character/Character.h"

/// @brief 実行
void InAttackSequence::Exec()
{
	// ブレークポイントのチェック
	BreakpointOnExec();

	// 基底の実行処理
	Action::Exec();

	// すでに攻撃シーケンス中なら失敗で終了
	if (owner_->IsInAttackSequence())
	{
		Exit();
		return;
	}

	// 攻撃シーケンス開始
	owner_->SetInAttackSequence(true);

	// ここまで来たら成功
	Action::Update();
}

/// @brief 終了、中断
void InAttackSequence::Exit()
{
	BreakpointOnExit();

	// 終了させる
	Action::Exit();
}

/// @brief リセット
void InAttackSequence::Reset()
{
	// ブレークポイントのチェック
	BreakpointOnReset();

	// 基底クラスのリセット
	Action::Reset();
}