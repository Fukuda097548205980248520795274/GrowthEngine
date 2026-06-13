#include "BehaviorTreeEditorHistory.h"
#include "../BehaviorTreeEditor.h"

/// @brief 現在の状態を履歴に保存する
/// @param nodes 
/// @param links 
/// @param currentId 
void BehaviorTreeEditorHistory::SaveHistory(const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links, int currentId)
{
    // 現在の状態をスナップショットとして保存
    EditorSnapshot snapshot;
    snapshot.nodes = nodes;
    snapshot.links = links;
    snapshot.currentId = currentId;

    undoHistory_.push_back(snapshot);
    redoHistory_.clear(); // 新しい操作が行われたらRedo履歴は破棄する

    // 履歴の最大数を制限（メモリ節約のため、例として50回まで）
    if (undoHistory_.size() > 50)
    {
        undoHistory_.erase(undoHistory_.begin());
    }
}

/// @brief Undo（元に戻す）を実行する
/// @param editor 
void BehaviorTreeEditorHistory::Undo(BehaviorTreeEditor& editor)
{
    if (undoHistory_.empty()) return;

	// 現在の状態をRedo履歴に保存しておく
    for (auto& node : editor.nodes_)
        node.needSetPos = true;

	// 選択されているノードのIDを保存
    std::vector<int> currentSelectedNodes;
    if (int numNodes = ImNodes::NumSelectedNodes(); numNodes > 0)
    {
        currentSelectedNodes.resize(numNodes);
        ImNodes::GetSelectedNodes(currentSelectedNodes.data());
    }

	// 選択されているリンクのIDも保存
    std::vector<int> currentSelectedLinks;
    if (int numLinks = ImNodes::NumSelectedLinks(); numLinks > 0)
    {
        currentSelectedLinks.resize(numLinks);
        ImNodes::GetSelectedLinks(currentSelectedLinks.data());
    }

    EditorSnapshot currentSnapshot = { editor.nodes_, editor.links_, editor.currentId_ };
    redoHistory_.push_back(currentSnapshot);

    // Undo履歴の最後尾から状態を復元
    EditorSnapshot snapshot = undoHistory_.back();
    undoHistory_.pop_back();

    editor.nodes_ = snapshot.nodes;
    editor.links_ = snapshot.links;
    editor.currentId_ = snapshot.currentId;

    // 復元した座標をImNodesに反映
    for (const auto& node : editor.nodes_)
    {
        ImNodes::SetNodeGridSpacePos(node.id, ImVec2(node.pos.x, node.pos.y));
    }

	// 復元した選択状態をImNodesに反映
    ImNodes::ClearNodeSelection();
    ImNodes::ClearLinkSelection();

	// ノードの選択状態を復元
    for (int id : snapshot.selectedNodes)
    {
        ImNodes::SelectNode(id);
    }

	// リンクの選択状態も復元
    for (int id : snapshot.selectedLinks)
    {
        ImNodes::SelectLink(id);
    }
}

/// @brief Redo（やり直す）を実行する
/// @param editor 
void BehaviorTreeEditorHistory::Redo(BehaviorTreeEditor& editor)
{
    if (redoHistory_.empty()) return;

    // 現在の状態をUndo履歴に保存しておく
    for (auto& node : editor.nodes_)
        node.needSetPos = true;

	// 選択されているノードのIDを保存
    std::vector<int> currentSelectedNodes;
    if (int numNodes = ImNodes::NumSelectedNodes(); numNodes > 0)
    {
        currentSelectedNodes.resize(numNodes);
        ImNodes::GetSelectedNodes(currentSelectedNodes.data());
    }

	// 選択されているリンクのIDも保存
    std::vector<int> currentSelectedLinks;
    if (int numLinks = ImNodes::NumSelectedLinks(); numLinks > 0)
    {
        currentSelectedLinks.resize(numLinks);
        ImNodes::GetSelectedLinks(currentSelectedLinks.data());
    }

    EditorSnapshot currentSnapshot = { editor.nodes_, editor.links_, editor.currentId_ };
    undoHistory_.push_back(currentSnapshot);

    // Redo履歴の最後尾から状態を復元
    EditorSnapshot snapshot = redoHistory_.back();
    redoHistory_.pop_back();

    editor.nodes_ = snapshot.nodes;
    editor.links_ = snapshot.links;
    editor.currentId_ = snapshot.currentId;

    // 復元した座標をImNodesに反映
    for (const auto& node : editor.nodes_)
    {
        ImNodes::SetNodeGridSpacePos(node.id, ImVec2(node.pos.x, node.pos.y));
    }

    ImNodes::ClearNodeSelection();
    ImNodes::ClearLinkSelection();

	// 復元した選択状態をImNodesに反映
    for (int id : snapshot.selectedNodes)
    {
        ImNodes::SelectNode(id);
    }

	// 復元した選択状態をImNodesに反映
    for (int id : snapshot.selectedLinks)
    {
        ImNodes::SelectLink(id);
    }
}

/// @brief 履歴をクリアする
void BehaviorTreeEditorHistory::Clear()
{
    undoHistory_.clear();
    redoHistory_.clear();
}