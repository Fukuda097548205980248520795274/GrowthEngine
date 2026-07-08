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
void CompositeNode::DrawDebuggerRecursive(float zoom)
{
    // まず自分自身を描画
    Node::DrawDebuggerRecursive(zoom);

    // 子ノードを巡回して描画し、リンク（線）を繋ぐ
    for (const auto& child : children_)
    {
        if (!child) continue;

        // 子ノードのデバッグ描画を再帰呼び出し
        child->DrawDebuggerRecursive(zoom);

		// リンクの色を子ノードの状態に応じて変更
		uint32_t linkColor = IM_COL32(60, 60, 60, 255); // None: グレー
		switch (child->GetLastState())
		{
		case State::Running:
			linkColor = IM_COL32(30, 180, 30, 255);  // Running: 緑
			break;
		case State::Success:
			linkColor = IM_COL32(30, 30, 180, 255);  // Success: 青
			break;
		case State::Failure:
			linkColor = IM_COL32(180, 30, 30, 255);  // Failure: 赤
			break;
		default:
			break;
		}

		// 状態がRunningなら線を太く、それ以外は通常
		float thickness = (child->GetLastState() == State::Running) ? 4.0f : 1.5f;
		ImNodes::PushStyleVar(ImNodesStyleVar_LinkThickness, thickness);

		// リンクの色を設定して描画
		ImNodes::PushColorStyle(ImNodesCol_Link, linkColor);
		ImNodes::Link(child->GetInputPinId(), this->outputPinId_, child->GetInputPinId());

		ImNodes::PopColorStyle();
		ImNodes::PopStyleVar(); // 太さのスタイルをポップ
    }
}