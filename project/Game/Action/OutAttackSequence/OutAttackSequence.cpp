#include "OutAttackSequence.h"
#include "Entity/Character/Character.h"

/// @brief 実行
void OutAttackSequence::Exec()
{
	// 攻撃シーケンス終了
	owner_->SetInAttackSequence(false);
}