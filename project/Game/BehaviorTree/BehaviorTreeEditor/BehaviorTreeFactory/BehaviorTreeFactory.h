#pragma once
#include "BehaviorTree/BehaviorTreeEditor/BehaviorTreeEditor.h"
#include "BehaviorTree/BehaviorTree.h"

class Character;

class BehaviorTreeFactory
{
public:

	/// @brief エディタ上のノードとリンクからビヘイビアツリーを構築する
	/// @param editor_nodes 
	/// @param editor_links 
	/// @param character 
	/// @return 
	static std::unique_ptr<BehaviorTree> CreateTree(const std::vector<EditorNode>& editorModes, const std::vector<EditorLink>& editorLinks,
		Character* character, const std::string& name);

private:

	/// @brief エディタ上のノードとリンクからルートノードを見つける
	/// @param nodes 
	/// @param links 
	/// @return 
	static const EditorNode* FindRootNode(const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links);

	/// @brief エディタ上のノードとリンクからノードを再帰的に構築する
	/// @param editor_node 
	/// @param nodes 
	/// @param links 
	/// @param character 
	/// @return 
	static std::unique_ptr<Node> BuildNodeRecursive(const EditorNode& editorNode, const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links, Character* character);
};

