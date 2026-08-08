#include "Telegraph.h"
#include "Entity/Character/Character.h"

/// @brief コンストラクタ
/// @param character 
/// @param initData 
Telegraph::Telegraph(Character* character, const TelegraphInitData& initData) : Action(character), time_(initData.time), hAnimation_(initData.hAnimation)
{

}

/// @brief デストラクタ
Telegraph::~Telegraph()
{
	// 予備動作中であれば、予備動作を終了する
	if (owner_ && owner_->GetCurrentTelegraph() == this)
		owner_->SetCurrentTelegraph(nullptr);
}

/// @brief 実行
void Telegraph::Exec()
{
	// ブレイクポイントのチェック
	BreakpointOnExec();

	// 予備動作ポインタを渡す
	owner_->SetCurrentTelegraph(this);

	// 状態なしに遷移する
	auto stateMachine = owner_->GetStateMachine();
	stateMachine->ChangeState("None");

	// タイマーをリセットする
	timer_ = 0.0f;

	// アニメーションを設定する
	owner_->SetAnimation(hAnimation_, true, false);

	// 移動を停止する
	owner_->MoveStop();

	// 基底クラスのExecを呼び出す
	Action::Exec();
}

/// @brief 更新処理
void Telegraph::Update()
{
	// ブレイクポイントのチェック
	BreakpointOnUpdate();

	// もし回避中、掴み中、無力化中であれば、予備動作を終了する
	if (owner_->IsJustAvoided() || owner_->IsGrabbing() || owner_->IsIncapacitated())
	{
		this->Exit();
		return;
	}

	// タイマーを進める
	timer_ += engine_->GetDeltaTime() * engine_->GetTimeScale();

	// 予備動作時間が経過した場合は成功
	if (timer_ >= time_)
	{
		Action::Update();
		return;
	}
}

/// @brief リセット
void Telegraph::Reset()
{
	// ブレイクポイントのチェック
	BreakpointOnReset();

	// もし現在の予備動作が自分自身であれば、予備動作ポインタをnullptrにする
	owner_->SetCurrentTelegraph(nullptr);

	Action::Reset();
}

/// @brief 終了、中断
void Telegraph::Exit()
{
	// ブレイクポイントのチェック
	BreakpointOnExit();

	if (owner_->GetCurrentTelegraph() == this)
	{
		// 予備動作ポインタをnullptrにする
		owner_->SetCurrentTelegraph(nullptr);
	}

	Action::Exit();
}

/// @brief 予備動作中かどうか
/// @return 
bool Telegraph::IsUse() const
{
	if (!IsExec()) return false;

	// 現在の予備動作が自分自身かどうか
	return this == owner_->GetCurrentTelegraph();
}