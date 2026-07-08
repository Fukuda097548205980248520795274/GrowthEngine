#include "ActionNode.h"

/// @brief 実行
/// @return 
Node::State ActionNode::Exec()
{
	// アクションがない場合は失敗
	if (!action_) return State::Failure;

	
	// アクションが実行されていない場合は実行する
	if (!action_->IsExec() && !action_->IsSuccess() && !action_->IsFailure())
		action_->Exec();

	// アクションが実行中の場合は、ノードの状態も実行中
	if (action_->IsExec())
		return State::Running;

	// アクションの状態に応じてノードの状態を返す
	State result = State::Failure;
	if (action_->IsSuccess())
	{
		result = State::Success;
	} 
	else if (action_->IsFailure() || !action_->IsUse())
	{
		result = State::Failure;
	}

	// アクションをリセットする
	action_->Reset();
	return result;
}

/// @brief 中断処理
void ActionNode::Abort()
{
	// アクションがない場合は何もしない
	if (!action_)return;

	// アクションが動いていたら強制終了
	if (action_->IsExec())
		action_->Exit();

	// アクションをリセットする
	action_->Reset();
}

/// @brief カスタムノードUIを描画する
/// @param zoom 
void ActionNode::DrawCustomNodeUI(float zoom)
{
	if (!action_) return;

	ImGui::PushID(editorNodeId_ + 10000);

	ImGui::Text("Breakpoints:");

	// Actionの構造体の参照を取得して直接チェックボックスと紐づける
	Action::Breakpoints& bp = action_->GetBreakpoints();

	// ImGui::SameLine() を使って2列×2行でコンパクトに配置
	ImGui::Checkbox("Exec", &bp.onExec);
	ImGui::SameLine();
	ImGui::Checkbox("Update", &bp.onUpdate);

	ImGui::Checkbox("Exit", &bp.onExit);
	ImGui::SameLine();
	ImGui::Checkbox("Reset", &bp.onReset);

	ImGui::PopID();
}