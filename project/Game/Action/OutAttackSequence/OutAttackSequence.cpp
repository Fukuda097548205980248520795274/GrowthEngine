#include "OutAttackSequence.h"
#include "Entity/Character/Character.h"

/// @brief 実行
void OutAttackSequence::Exec()
{
	// ブレークポイントのチェック
	BreakpointOnExec();

	// 基底の実行処理
	Action::Exec();

	// 攻撃シーケンス終了
	owner_->SetInAttackSequence(false);

	// ここまで来たら成功
	Action::Update();
}

/// @brief 終了、中断
void OutAttackSequence::Exit()
{
	// ブレークポイントのチェック
	BreakpointOnExit();

	// 基底の終了処理
	Action::Exit();
}

/// @brief リセット
void OutAttackSequence::Reset()
{
	// ブレークポイントのチェック
	BreakpointOnReset();

	// 攻撃シーケンス終了
	owner_->SetInAttackSequence(false);

	// 基底クラスのリセット
	Action::Reset();
}