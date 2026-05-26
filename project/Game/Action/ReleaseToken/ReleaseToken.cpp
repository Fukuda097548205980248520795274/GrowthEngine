#include "ReleaseToken.h"
#include "Entity/Character/Character.h"
#include "BattleDirector/BattleDirector.h"

/// @brief 実行
void ReleaseToken::Exec()
{
	// すでに実行されている場合は何もしない
	if (IsExec()) return;

	// 基底クラスの実行
	Action::Exec();

	// 攻撃トークンを返却する
	BattleDirector::GetInstance().ReleaseAttackToken(owner_);
}

/// @brief 更新処理
void ReleaseToken::Update()
{
	// 実行されていない場合は何もしない
	if (!IsExec()) return;

	// 終了した場合は、成功フラグを立ててExit()を呼ぶ
	Action::Update();
}

/// @brief 使用中かどうか
/// @return 
bool ReleaseToken::IsUse() const
{
	// 実行していない場合は使用していないとみなす
	if (!IsExec()) return false;

	
	return true;
}