#include "Move.h"
#include "Entity/Character/Character.h"

/// @brief デストラクタ
Move::~Move()
{
	// 自分が現在の移動処理として登録されている場合のみ、停止とクリアを行う
	if (owner_->GetCurrentMove() == this)
		owner_->SetCurrentMove(nullptr);
}

/// @brief 実行
void Move::Exec()
{
	// 移動ポインタを渡す
	owner_->SetCurrentMove(this);

	// 基底クラスのExecを呼び出す
	Action::Exec();
}

/// @brief 更新処理
void Move::Update()
{
	// 基底クラスのUpdateを呼び出す
	Action::Update();
}

/// @brief 中断・終了
void Move::Exit()
{
	// ブレークポイントのチェック
	BreakpointOnExit();

	// 自分が現在の移動処理として登録されている場合のみ、停止とクリアを行う
	if (owner_->GetCurrentMove() == this)
	{
		// 移動入力をリセットする
		if(owner_->IsStance())owner_->SetMoveInputXZ(Vector2(0.0f, 0.0f), 0.0f);
		else owner_->MoveStop();

		// 移動を停止する
		owner_->SetCurrentMove(nullptr);
	}

	// 基底クラスのExitを呼び出す
	Action::Exit();
}

bool Move::IsUse() const
{
	// 実行していない場合は使用していないとみなす
	if (!IsExec())return false;

	return this == owner_->GetCurrentMove();
}