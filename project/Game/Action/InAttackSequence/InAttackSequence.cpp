#include "InAttackSequence.h"
#include "Entity/Character/Character.h"

/// @brief 実行
void InAttackSequence::Exec()
{
	// 攻撃シーケンスがすでに開始されている場合はエラー
	if (owner_->IsInAttackSequence())
	{
		assert(false && "攻撃シーケンスが停止されていません。");
		return;
	}

	// 攻撃シーケンス開始
	owner_->SetInAttackSequence(true);
}