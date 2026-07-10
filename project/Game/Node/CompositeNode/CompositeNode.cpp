#include "CompositeNode.h"

/// @brief 中断処理
void CompositeNode::Abort()
{
	for (auto& child : children_)
		child->Abort();
}

#ifdef _DEVELOPMENT

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

		// 変更：リンクの色と太さを決定
		uint32_t linkColor = IM_COL32(60, 60, 60, 255); // None: グレー
		float thickness = 1.5f;

		Node::State childState = child->GetLastState();

		if (childState == Node::State::Running)
		{
			linkColor = IM_COL32(30, 180, 30, 255);  // Running: 緑
			thickness = 4.0f; // Running時は線を太く
		}
		else if (childState == Node::State::Success)
		{
			linkColor = IM_COL32(30, 30, 180, 255);  // Success: 青
		}
		else if (childState == Node::State::Failure)
		{
			linkColor = IM_COL32(180, 30, 30, 255);  // Failure: 赤
		} 
		else if (child->GetFadeTimer() > 0.0f)
		{
			// リンクも残像フェードさせる
			float t = child->GetFadeTimer() / 1.0f; // Node.hのFADE_DURATIONに合わせる
			Node::State fadeState = child->GetFadeState();

			int targetR = (fadeState == Node::State::Success) ? 30 : 180;
			int targetG = 30;
			int targetB = (fadeState == Node::State::Success) ? 180 : 30;

			int r = static_cast<int>(60.0f + (targetR - 60.0f) * t);
			int g = static_cast<int>(60.0f + (targetG - 60.0f) * t);
			int b = static_cast<int>(60.0f + (targetB - 60.0f) * t);

			linkColor = IM_COL32(r, g, b, 255);
		}

		ImNodes::PushStyleVar(ImNodesStyleVar_LinkThickness, thickness);

		// リンクの色を設定して描画
		ImNodes::PushColorStyle(ImNodesCol_Link, linkColor);
		ImNodes::Link(child->GetInputPinId(), this->outputPinId_, child->GetInputPinId());

		ImNodes::PopColorStyle();
		ImNodes::PopStyleVar(); // 太さのスタイルをポップ
    }
}

#endif