#include "BehaviorTreeBuilder.h"
#include <functional>

/// @brief セレクタノードを追加する
/// @return 
BehaviorTreeBuilder& BehaviorTreeBuilder::PersistentSelector()
{
	nodes_.push_back(std::make_unique<PersistentSelectorNode>());
	return *this;
}

/// @brief シーケンスノードを追加する
/// @return
BehaviorTreeBuilder& BehaviorTreeBuilder::PersistentSequence()
{
	nodes_.push_back(std::make_unique<PersistentSequenceNode>());
	return *this;
}

/// @brief セレクタノードを追加する
/// @return 
BehaviorTreeBuilder& BehaviorTreeBuilder::RestartingSelector()
{
	nodes_.push_back(std::make_unique<RestartingSelectorNode>());
	return *this;
}

/// @brief シーケンスノードを追加する
/// @return 
BehaviorTreeBuilder& BehaviorTreeBuilder::RestartingSequence()
{
	nodes_.push_back(std::make_unique<RestartingSequenceNode>());
	return *this;
}

/// @brief 条件ノードを追加する
/// @param func 
/// @return 
BehaviorTreeBuilder& BehaviorTreeBuilder::Condition(std::function<bool()> func)
{
	nodes_.push_back(std::make_unique<ConditionNode>(func));
	return *this;
}

/// @brief アクションノードを追加する
/// @param action 
/// @return 
BehaviorTreeBuilder& BehaviorTreeBuilder::Action_(std::unique_ptr<Action> action)
{
	nodes_.push_back(std::make_unique<ActionNode>(std::move(action)));
	return *this;
}

/// @brief コンボ攻撃ノードを追加する
/// @param comboAttack 
/// @return 
BehaviorTreeBuilder& BehaviorTreeBuilder::ComboAttack_(std::unique_ptr<ComboAttack> comboAttack)
{
	nodes_.push_back(std::make_unique<ComboAttackNode>(std::move(comboAttack)));
	return *this;
}

/// @brief つかみ攻撃ノードを追加する
/// @param grabAttack 
/// @return 
BehaviorTreeBuilder& BehaviorTreeBuilder::GrabAttack_(std::unique_ptr<GrabAttack> grabAttack)
{
	nodes_.push_back(std::make_unique<GrabAttackNode>(std::move(grabAttack)));
	return *this;
}

/// @brief ノードの追加を終了する
/// @return 
BehaviorTreeBuilder& BehaviorTreeBuilder::End()
{
	// 現在のルートノードが複数あるとき
	if (nodes_.size() > 1)
	{
		// 最新のノードを取得する
		auto node = std::move(nodes_.back());
		nodes_.pop_back();
		PrevNodeAddChild(std::move(node));
	}

	return *this;
}

/// @brief ツリーをビルドする
/// @return 
std::unique_ptr<Node> BehaviorTreeBuilder::Build()
{
	// ルートノードは1つのみ
	assert(nodes_.size() == 1 && "BehaviorTree Build Faild");
	return std::move(nodes_.front());
}

/// @brief 最新のノードにつなげる
/// @param child 
void BehaviorTreeBuilder::PrevNodeAddChild(std::unique_ptr<Node> child)
{
	// コンポジットノードのみ
	auto* composite = dynamic_cast<CompositeNode*>(nodes_.back().get());
	assert(composite && "not Composite");
	composite->AddChild(std::move(child));
}