#include "ComboTreeEditorClipboard.h"
#include "../ComboTreeEditor.h"
#include <unordered_set>

/// @brief コピーしたノードとリンクの情報をクリップボードに保存する
/// @param sourceNodes 
/// @param sourceLinks 
void ComboTreeEditorClipboard::HandleCopy(const std::vector<ComboEditorNode>& sourceNodes, const std::vector<ComboEditorLink>& sourceLinks)
{
    clipboardNodes_.clear();
    clipboardLinks_.clear();

    // 選択されているノードの数を取得
    int numSelectedNodes = ImNodes::NumSelectedNodes();
    if (numSelectedNodes == 0) return;

    std::vector<int> selectedNodeIds(numSelectedNodes);
    ImNodes::GetSelectedNodes(selectedNodeIds.data());

    // 検索を高速化するためのSet
    std::unordered_set<int> selectedSet(selectedNodeIds.begin(), selectedNodeIds.end());
    std::unordered_set<int> copiedPinIds; // コピーしたピンのID一覧（リンクの判定用）

    // ノードのコピー
    for (const auto& node : sourceNodes)
    {
        if (selectedSet.count(node.id))
        {
            // ノードを複製
            ComboEditorNode copyNode = node;

			// ノードの現在の座標を取得してコピー先に設定
            ImVec2 currentPos = ImNodes::GetNodeGridSpacePos(node.id);
            copyNode.pos = currentPos;

            clipboardNodes_.push_back(copyNode);

			// コピーしたノードのピンIDを記録
            copiedPinIds.insert(node.inputPinId);
            copiedPinIds.insert(node.outputInputXPinId);
            copiedPinIds.insert(node.outputInputYPinId);
			copiedPinIds.insert(node.outputInputBPinId);
        }
    }

    // リンクのコピー (両端のピンがどちらもコピー対象ノードに含まれている場合のみ)
    for (const auto& link : sourceLinks)
    {
        if (copiedPinIds.count(link.startPinId) && copiedPinIds.count(link.endPinId))
        {
            clipboardLinks_.push_back(link);
        }
    }
}

/// @brief コピーしたノードとリンクの情報をエディタにペーストする
/// @param editor 
void ComboTreeEditorClipboard::HandlePaste(ComboTreeEditor& editor)
{
    if (clipboardNodes_.empty()) return;

    /// @brief 変更があったことを通知する関数
    editor.HandleChange();

    // 現在のウィンドウ（ノードエディタ）の位置とサイズを取得して中央の座標を計算
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 centerPos = ImVec2(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);

    // 旧ピンIDから新ピンIDへのマッピング (リンクの再接続に必要)
    std::unordered_map<int, int> pinIdMap;

    ImNodes::ClearNodeSelection(); // 既存の選択を解除

    bool isFirst = true;
    ImVec2 gridOffset(0.0f, 0.0f);

    // ノードのペースト
    for (auto node : clipboardNodes_)
    {
        // エディタ側で新しいIDを発行
        int newId = editor.GetNextId();
        int newInputPinId = editor.GetNextId();
        int newOutInputXPinId = editor.GetNextId();
        int newOutInputYPinId = editor.GetNextId();
		int newOutInputBPinId = editor.GetNextId();

        // リンクを繋ぎ直すために、元のピンIDと新しいピンIDのペアを記憶
        pinIdMap[node.inputPinId] = newInputPinId;
        pinIdMap[node.outputInputXPinId] = newOutInputXPinId;
        pinIdMap[node.outputInputYPinId] = newOutInputYPinId;
		pinIdMap[node.outputInputBPinId] = newOutInputBPinId;

        // ノード本体のIDを更新
        node.id = newId;
        node.inputPinId = newInputPinId;
        node.outputInputXPinId = newOutInputXPinId;
        node.outputInputYPinId = newOutInputYPinId;
		node.outputInputBPinId = newOutInputBPinId;

        // ノードの位置を更新
        if (isFirst)
        {
            // 最初のノードを画面の中央（スクリーンスペース）に強制配置
            ImNodes::SetNodeScreenSpacePos(node.id, centerPos);

            // 配置された後の内部グリッド座標を取得
            ImVec2 newGridPos = ImNodes::GetNodeGridSpacePos(node.id);

            // コピー元からどれだけグリッドが移動したか（オフセット）を算出
            gridOffset = newGridPos - node.pos;

            // ノードの座標を更新
            node.pos = newGridPos;
            isFirst = false;
        }
        else
        {
            // 2番目以降のノードは、最初のノードと同じ移動量を適用
            node.pos += gridOffset;
            ImNodes::SetNodeGridSpacePos(node.id, node.pos);
        }

        // エディタへ追加
        editor.nodes_.push_back(node);

        // ImNodesの表示座標を更新し、ペーストしたノードを選択状態にする
        ImNodes::SetNodeGridSpacePos(node.id, node.pos);
        ImNodes::SelectNode(node.id);
    }

    // リンクのペースト
    for (auto link : clipboardLinks_)
    {
        // 記録したマップから新しいピンIDを取得して繋ぐ
        if (pinIdMap.count(link.startPinId) && pinIdMap.count(link.endPinId))
        {
            link.id = editor.GetNextId();
            link.startPinId = pinIdMap[link.startPinId];
            link.endPinId = pinIdMap[link.endPinId];
            editor.links_.push_back(link);
        }
    }
}