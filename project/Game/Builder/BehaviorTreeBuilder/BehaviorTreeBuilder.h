#pragma once
#include "Node/CompositeNode/PersistentSelectorNode/PersistentSelectorNode.h"
#include "Node/CompositeNode/PersistentSequenceNode/PersistentSequenceNode.h"
#include "Node/CompositeNode/RestartingSelectorNode/RestartingSelectorNode.h"
#include "Node/CompositeNode/RestartingSequenceNode/RestartingSequenceNode.h"
#include "Node/ConditionNode/ConditionNode.h"
#include "Node/ActionNode/ActionNode.h"
#include "Action/Action.h"

class BehaviorTreeBuilder
{
public:

	/// @brief セレクタノードを追加する
	/// @return 
	BehaviorTreeBuilder& PersistentSelector();

	/// @brief シーケンスノードを追加する
	/// @return 
	BehaviorTreeBuilder& PersistentSequence();

	/// @brief セレクタノードを追加する
	/// @return 
	BehaviorTreeBuilder& RestartingSelector();

	/// @brief シーケンスノードを追加する
	/// @return
	BehaviorTreeBuilder& RestartingSequence();

	/// @brief 条件ノードを追加する
	/// @param func 
	/// @return 
	BehaviorTreeBuilder& Condition(std::function<bool()> func);

	/// @brief アクションノードを追加する
	/// @param action 
	/// @return 
	BehaviorTreeBuilder& Action_(std::unique_ptr<Action> action);

	/// @brief ノードの追加を終了する
	/// @return 
	BehaviorTreeBuilder& End();

	/// @brief ツリーをビルドする
	/// @return 
	std::unique_ptr<Node> Build();


private:

	/// @brief 最新のノードにつなげる
	/// @param child 
	void PrevNodeAddChild(std::unique_ptr<Node> child);

	/// @brief 作成中のノード
	std::vector<std::unique_ptr<Node>> nodes_;
};

