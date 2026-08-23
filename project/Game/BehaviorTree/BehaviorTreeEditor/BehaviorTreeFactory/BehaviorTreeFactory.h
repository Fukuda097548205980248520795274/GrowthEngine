#pragma once
#include "BehaviorTree/BehaviorTreeEditor/BehaviorTreeEditor.h"
#include "BehaviorTree/BehaviorTree.h"

class Character;

class BehaviorTreeFactory
{
public:

	/// @brief エディタ上のノードとリンクからビヘイビアツリーを構築する
	/// @param editorNodes 
	/// @param editorLinks 
	/// @param character 
	/// @return 
	static std::unique_ptr<BehaviorTree> CreateTree(const std::vector<EditorNode>& editorNodes, const std::vector<EditorLink>& editorLinks,
		Character* character, const std::string& name);

private:

	/// @brief エディタ上のノードとリンクからルートノードを見つける
	/// @param nodes 
	/// @param links 
	/// @return 
	static const EditorNode* FindRootNode(const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links);

	/// @brief エディタ上のノードとリンクからノードを再帰的に構築する
	/// @param editorNode 
	/// @param nodes 
	/// @param links 
	/// @param character 
	/// @return 
	static std::unique_ptr<Node> BuildNodeRecursive(const EditorNode& editorNode, const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links, Character* character);

	/// @brief 指定されたピンIDに対応するノードIDを取得する
	/// @param nodes 
	/// @param pinId 
	/// @return 
	static int GetNodeIdFromPinId(const std::vector<EditorNode>& nodes, int pinId);

	/// @brief 指定された親ノードIDに対応する子ノードのIDを取得する
	/// @param nodes 
	/// @param links 
	/// @param parentId 
	/// @return 
	static std::vector<int> GetChildNodeIds(const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links, int parentId);

	/// @brief 指定されたノードIDに対応するEditorNodeを取得する
	/// @param nodes 
	/// @param nodeId 
	/// @return 
	static const EditorNode& GetEditorNode(const std::vector<EditorNode>& nodes, int nodeId);
};

