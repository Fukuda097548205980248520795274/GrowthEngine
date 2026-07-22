#include "Action.h"
#include "Entity/Character/Character.h"

/// @brief 攻撃の実行
void Action::Exec()
{
	// すでに攻撃が実行されている場合は何もしない
	if (isExec_)return;

	// 攻撃を実行したフラグを立てる
	isExec_ = true;

	// 成功フラグを初期化する
	isSuccess_ = false;

	// 失敗フラグを初期化する
	isFailure_ = false;
}

/// @brief 更新処理
void Action::Update()
{
	// 攻撃が成功した場合は、成功フラグを立てる
	isSuccess_ = true;

	// 攻撃を終了させる
	Exit();
}

/// @brief リセット
void Action::Reset()
{
	// フラグを初期化する
	isExec_ = false;
	isSuccess_ = false;
	isFailure_ = false;
}

/// @brief 攻撃の終了、中断
void Action::Exit()
{
	// 実行フラグを下す
	isExec_ = false;

	// 成功していない場合は、失敗フラグを立てる
	if (!isSuccess_)
		isFailure_ = true;
}

/// @brief ブレークポイントのチェック
void Action::BreakpointOnExec()
{
#ifdef DEVELOPMENT
	if (breakpoints_.onExec)
	{
		__debugbreak();
		breakpoints_.onExec = false; // 一度ブレークしたらフラグを下す
	}
#endif
}

/// @brief ブレークポイントのチェック
void Action::BreakpointOnUpdate()
{
#ifdef DEVELOPMENT
	if (breakpoints_.onUpdate)
	{
		__debugbreak();
		breakpoints_.onUpdate = false; // 一度ブレークしたらフラグを下す
	}
#endif
}

/// @brief ブレークポイントのチェック
void Action::BreakpointOnExit()
{
#ifdef DEVELOPMENT
	if (breakpoints_.onExit)
	{
		__debugbreak();
		breakpoints_.onExit = false; // 一度ブレークしたらフラグを下す
	}
#endif
}

/// @brief ブレークポイントのチェック
void Action::BreakpointOnReset()
{
#ifdef DEVELOPMENT
	if (breakpoints_.onReset)
	{
		__debugbreak();
		breakpoints_.onReset = false; // 一度ブレークしたらフラグを下す
	}
#endif
}