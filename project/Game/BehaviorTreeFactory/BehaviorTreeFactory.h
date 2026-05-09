#pragma once
#include "BehaviorTreeEditor/BehaviorTreeEditor.h"
#include "BehaviorTree/BehaviorTree.h"

class BehaviorTreeFactory
{
public:

	/// @brief エディタ上のノードとリンクからビヘイビアツリーを生成する
    /// @param editor_nodes 
    /// @param editor_links 
    /// @return 
    static std::unique_ptr<BehaviorTree> CreateTree(const std::vector<EditorNode>& editor_nodes, const std::vector<EditorLink>& editor_links);

private:

	/// @brief エディタ上のノードとリンクからルートノードを見つける
    /// @param nodes 
    /// @param links 
    /// @return 
    static const EditorNode* FindRootNode(const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links);

	/// @brief エディタ上のノードとリンクから再帰的にランタイムノードを構築する
    /// @param editor_node 
    /// @param nodes 
    /// @param links 
    /// @return 
    static std::unique_ptr<Node> BuildNodeRecursive(const EditorNode& editor_node, const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links);
};

