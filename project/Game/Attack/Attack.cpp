#include "Attack.h"
#include "Entity/Character/Character.h"

/// @brief 攻撃の実行
void Attack::Exec()
{
	// すでに攻撃が実行されている場合は何もしない
	if (isExec_)return;

	// キャラクターに攻撃をセットする
	owner_->SetCurrentAttack(this);

	// 攻撃を実行したフラグを立てる
	isExec_ = true;

	// 成功フラグを初期化する
	isSuccess_ = false;

	// 失敗フラグを初期化する
	isFailure_ = false;
}

/// @brief 更新処理
void Attack::Update()
{
	// 攻撃が成功した場合は、成功フラグを立てる
	isSuccess_ = true;

	// 攻撃を終了させる
	Exit();
}

/// @brief 攻撃の終了、中断
void Attack::Exit()
{
	// 実行フラグを下す
	isExec_ = false;

	// 成功していない場合は、失敗フラグを立てる
	if (!isSuccess_)
		isFailure_ = true;

	// キャラクターの攻撃を解除する
	owner_->SetCurrentAttack(nullptr);
}