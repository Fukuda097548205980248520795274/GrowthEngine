#include "CompositeNode.h"

/// @brief 中断処理
void CompositeNode::Abort()
{
	for (auto& child : children_)
		child->Abort();
}

/// @brief デバッグ状態のリセット（毎フレームのツリー更新の先頭で呼ぶ）
void CompositeNode::ResetStatusRecursive()
{
	Node::ResetStatusRecursive();

	for (auto& child : children_) 
	{
		if (child) child->ResetStatusRecursive();
	}
}

/// @brief デバッグ用の再帰描画処理
void CompositeNode::DrawDebuggerRecursive()
{
    // まず自分自身を描画
    Node::DrawDebuggerRecursive();

    // 子ノードを巡回して描画し、リンク（線）を繋ぐ
    for (const auto& child : children_)
    {
        if (!child) continue;

        // 子ノードのデバッグ描画を再帰呼び出し
        child->DrawDebuggerRecursive();

        // 自分（出力ピン）から子（入力ピン）へのリンクを描画
        // 入力ピンIDはツリー内で必ずユニークなため、そのままLinkIdとして安全に流用できます
        ImNodes::Link(child->GetInputPinId(), this->outputPinId_, child->GetInputPinId());
    }
}