#include "OutAttackSequence.h"
#include "Entity/Character/Character.h"

/// @brief 実行
void OutAttackSequence::Exec()
{
	// ブレークポイントのチェック
	BreakpointOnExec();

	// 攻撃シーケンス終了
	owner_->SetInAttackSequence(false);
}