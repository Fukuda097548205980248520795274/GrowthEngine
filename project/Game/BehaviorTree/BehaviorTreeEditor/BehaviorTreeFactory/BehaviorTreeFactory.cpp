#include "BehaviorTreeFactory.h"
#include "Entity/Character/Character.h"
#include "BattleDirector/BattleDirector.h"
#include "Node/ConditionNode/ConditionNode.h"
#include "Node/CompositeNode/PersistentSelectorNode/PersistentSelectorNode.h"
#include "Node/CompositeNode/PersistentSequenceNode/PersistentSequenceNode.h"
#include "Node/CompositeNode/RestartingSelectorNode/RestartingSelectorNode.h"
#include "Node/CompositeNode/RestartingSequenceNode/RestartingSequenceNode.h"
#include "Node/ActionNode/ComboAttackNode/ComboAttackNode.h"
#include "Node/ActionNode/GrabAttackNode/GrabAttackNode.h"
#include "Node/ActionNode/GrabStrikeAttackNode/GrabStrikeAttackNode.h"
#include "Node/ActionNode/RequestTokenNode/RequestTokenNode.h"
#include "Node/ActionNode/ReleaseTokenNode/ReleaseTokenNode.h"
#include "Node/ActionNode/InAttackSequenceNode/InAttackSequenceNode.h"
#include "Node/ActionNode/OutAttackSequenceNode/OutAttackSequenceNode.h"

/// @brief エディタ上のノードとリンクからビヘイビアツリーを生成する
/// @param editor_nodes 
/// @param editor_links 
/// @return 
std::unique_ptr<BehaviorTree> BehaviorTreeFactory::CreateTree(const std::vector<EditorNode>& editor_nodes, const std::vector<EditorLink>& editor_links, Character* character)
{
	// ルートノードを見つける
	const EditorNode* root_editor_node = FindRootNode(editor_nodes, editor_links);

	// ルートノードが見つからない場合は nullptr を返す
	if (!root_editor_node) return nullptr;

	// ルートノードから再帰的にランタイムノードを構築する
	std::unique_ptr<Node> root_runtime_node = BuildNodeRecursive(*root_editor_node, editor_nodes, editor_links, character);

	// ルートノードが構築できなかった場合は nullptr を返す
	return std::make_unique<BehaviorTree>(std::move(root_runtime_node));
}

/// @brief エディタ上のノードとリンクからルートノードを見つける
/// @param nodes 
/// @param links 
/// @return 
const EditorNode* BehaviorTreeFactory::FindRootNode(const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links)
{
	// すべてのノードをチェックして、入力ピンにリンクがつながっていないノードを探す
	for (const auto& node : nodes)
	{
		bool has_input_link = false;
		for (const auto& link : links)
		{
			// ノードの入力ピンにリンクがつながっているかをチェック
			if (link.endPinId == node.inputPinId)
			{
				has_input_link = true;
				break;
			}
		}
		
		// 入力ピンにリンクがつながっていないノードがルートノード
		if (!has_input_link) return &node;
	}
	return nullptr;
}

/// @brief エディタ上のノードとリンクから再帰的にランタイムノードを構築する
/// @param editor_node 
/// @param nodes 
/// @param links 
/// @return 
std::unique_ptr<Node> BehaviorTreeFactory::BuildNodeRecursive(const EditorNode& editor_node, const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links, Character* character)
{
	// editor_node の種類に応じて対応するランタイムノードを生成する
	std::unique_ptr<Node> runtime_node = nullptr;

	// 条件関数の宣言（条件ノードの場合に使用）
	std::function<bool()> conditionFunc{};

	// ノードの種類に応じて対応するランタイムノードを生成
	switch (editor_node.type)
	{
	case EditorNodeType::RestartingSelector:
		runtime_node = std::make_unique<RestartingSelectorNode>();
		break;

	case EditorNodeType::PersistentSelector:
		runtime_node = std::make_unique<PersistentSelectorNode>();
		break;

	case EditorNodeType::RestartingSequence:
		runtime_node = std::make_unique<RestartingSequenceNode>();
		break;

	case EditorNodeType::PersistentSequence:
		runtime_node = std::make_unique<PersistentSequenceNode>();
		break;

	case EditorNodeType::Condition:

		// エディタで設定した条件の種類に応じて、条件関数を生成する
		switch (editor_node.conditionType)
		{
		// 関数なし、常に true を返す条件
		case ConditionType::None:
		default:
			conditionFunc = []() { return true; };
			break;

		// ターゲットがいるかどうかをチェックする条件
		case ConditionType::HasTarget:
			conditionFunc = [character]() { return character->HasTarget(); };
			break;

		// ターゲットがダウンしているかどうかをチェックする条件
		case ConditionType::IsTargetDown:
			conditionFunc = [character]() { return character->HasTarget() && character->GetLockOnTarget()->IsDown(); };
			break;

		// ターゲットがダウンしていないかどうかをチェックする条件
		case ConditionType::IsNotTargetDown:
			conditionFunc = [character]() { return !character->HasTarget() || !character->GetLockOnTarget()->IsDown(); };
			break;

			// 掴んでいるかどうかをチェックする条件
		case ConditionType::IsGrabbing:
			conditionFunc = [character]() { return character->IsGrabbing(); };
			break;

			// 掴んでいないかどうかをチェックする条件
		case ConditionType::IsNotGrabbing:
			conditionFunc = [character]() { return !character->IsGrabbing(); };
			break;

			// ターゲットが一定距離内にいるかどうかをチェックする条件
		case ConditionType::IsTargetInRange:
			conditionFunc = [character, editor_node]() 
				{
					if (!character->HasTarget()) return false;
					float distance = (character->GetLockOnTarget()->GetWorldPosition() - character->GetWorldPosition()).Length();
					return distance <= editor_node.conditionParam.distanceToTarget;
				};
			break;

			// ターゲットが一定距離外にいるかどうかをチェックする条件
		case ConditionType::IsTargetOutOfRange:
			conditionFunc = [character, editor_node]()
				{
					if (!character->HasTarget()) return true;
					float distance = (character->GetLockOnTarget()->GetWorldPosition() - character->GetWorldPosition()).Length();
					return distance > editor_node.conditionParam.distanceToTarget;
				};
			break;

			// ターゲットが攻撃しているかどうかをチェックする条件
		case ConditionType::IsTargetAttacking:
			conditionFunc = [character]() 
				{ 
					if (!character->HasTarget()) return false;
					return character->GetLockOnTarget()->IsAttack();
				};
			break;

			// ターゲットが攻撃していないかどうかをチェックする条件
		case ConditionType::IsTargetNotAttacking:
			conditionFunc = [character]()
				{
					if (!character->HasTarget()) return true;
					return !character->GetLockOnTarget()->IsAttack();
				};
			break;

			// ターゲットが攻撃動作中かどうかをチェックする条件
		case ConditionType::IsTargetInAttackSequence:
			conditionFunc = [character]()
				{
					if (!character->HasTarget()) return false;
					return character->GetLockOnTarget()->IsInAttackSequence();
				};
			break;

			// ターゲットが攻撃動作中でないかどうかをチェックする条件
		case ConditionType::IsTargetNotInAttackSequence:
			conditionFunc = [character]()
				{
					if (!character->HasTarget()) return true;
					return !character->GetLockOnTarget()->IsInAttackSequence();
				};
			break;

			// ターゲットに最も近いかどうかをチェックする条件
		case ConditionType::IsClosestToTarget:
			conditionFunc = [character]()
				{
					if (!character->HasTarget()) return false;
					return BattleDirector::GetInstance().IsClosestToTarget(character);
				};
			break;

			// ターゲットに最も近くないかどうかをチェックする条件
		case ConditionType::IsNotClosestToTarget:
			conditionFunc = [character]()
				{
					if (!character->HasTarget()) return true;
					return !BattleDirector::GetInstance().IsClosestToTarget(character);
				};
			break;
		}

		// 実際は editor_node.condition_name 等をもとに、
		// 登録済みの関数辞書から std::function を引いてきてバインドします
		runtime_node = std::make_unique<ConditionNode>(conditionFunc);
		break;

	case EditorNodeType::Action:

		// エディターで設定した文字列（actionName）に応じて生成するノードを変える
		if (editor_node.actionName == "ComboAttack")
		{
			// エディタ上で設定した初期化データを使用
			CombAttackInitData initData = editor_node.comboAttackInitData;

			auto comboAction = std::make_unique<ComboAttack>(character, initData);
			runtime_node = std::make_unique<ComboAttackNode>(std::move(comboAction));
		}
		else if (editor_node.actionName == "GrabAttack")
		{
			// エディタ上で設定した初期化データを使用
			GrabAttackInitData initData = editor_node.grabAttackInitData;

			auto grabAction = std::make_unique<GrabAttack>(character, initData);
			runtime_node = std::make_unique<GrabAttackNode>(std::move(grabAction));
		}
		else if (editor_node.actionName == "GrabStrikeAttack")
		{
			// エディタ上で設定した初期化データを使用
			GrabStrikeAttackInitData initData = editor_node.grabStrikeAttackInitData;
			auto grabStrikeAction = std::make_unique<GrabStrikeAttack>(character, initData);
			runtime_node = std::make_unique<GrabStrikeAttackNode>(std::move(grabStrikeAction));
		}
		else if (editor_node.actionName == "RequestToken")
		{
			// トークン要求ノードの生成
			runtime_node = std::make_unique<RequestTokenNode>(std::make_unique<RequestToken>(character));
		} 
		else if (editor_node.actionName == "ReleaseToken")
		{
			// トークン解放ノードの生成
			runtime_node = std::make_unique<ReleaseTokenNode>(std::make_unique<ReleaseToken>(character));
		}
		else if (editor_node.actionName == "Avoid")
		{
			// 回避ノードの生成
			runtime_node = std::make_unique<ActionNode>(std::make_unique<Avoid>(character, editor_node.avoidInitData));
		}
		else if (editor_node.actionName == "ApproachTargetMove")
		{
			// ターゲットに近づく移動ノードの生成
			runtime_node = std::make_unique<ActionNode>(std::make_unique<ApproachTargetMove>(character, editor_node.approachTargetMoveInitData));
		}
		else if (editor_node.actionName == "NavMeshMove")
		{
			// NavMeshを使用した移動ノードの生成
			runtime_node = std::make_unique<ActionNode>(std::make_unique<NavMeshMove>(character, editor_node.navMeshMoveInitData));
		}
		else if (editor_node.actionName == "InAttackSequence")
		{
			// 攻撃シーケンス開始ノードの生成
			runtime_node = std::make_unique<InAttackSequenceNode>(std::make_unique<InAttackSequence>(character));
		}
		else if (editor_node.actionName == "OutAttackSequence")
		{
			// 攻撃シーケンス終了ノードの生成
			runtime_node = std::make_unique<OutAttackSequenceNode>(std::make_unique<OutAttackSequence>(character));
		}
		else
		{
			// 何も設定されていない、または該当しない場合（何もしないノードにする等）
			runtime_node = nullptr;
		}

		break;
	}

	// ランタイムノードが生成できなかった場合は nullptr を返す
	if (auto composite_node = dynamic_cast<CompositeNode*>(runtime_node.get()))
	{
		std::vector<const EditorNode*> child_editor_nodes;

		// editor_node の出力ピンからつながっているリンクを探す
		for (const auto& link : links)
		{
			if (link.startPinId == editor_node.outputPinId)
			{
				// このリンクは editor_node の出力ピンから始まっているので、子ノードの入力ピンにつながっている
				for (const auto& n : nodes)
				{
					if (n.inputPinId == link.endPinId)
					{
						child_editor_nodes.push_back(&n);
						break;
					}
				}
			}
		}

		// 子ノードをY座標でソートして、エディタ上の見た目の順番で実行されるようにする
		std::sort(child_editor_nodes.begin(), child_editor_nodes.end(),
			[](const EditorNode* a, const EditorNode* b) {
				// ImNodesの関数を使わず、ロード済みの EditorNode のデータ(pos)を直接比較する
				return a->pos.y < b->pos.y;
			});

		// 子ノードを再帰的に構築してコンポジットノードに追加する
		for (const auto* child_node : child_editor_nodes)
		{
			auto child_runtime = BuildNodeRecursive(*child_node, nodes, links, character);
			if (child_runtime)
			{
				composite_node->AddChild(std::move(child_runtime));
			}
		}
	}


	// ラインタイムでノードを確認するためのデバッグ情報を設定
	if (runtime_node)
	{
		// ビューアー上に表示するノード名を設定
		std::string nodeName = "未設定";
		if (editor_node.type == EditorNodeType::Action) nodeName = editor_node.actionName;
		else if (editor_node.type == EditorNodeType::Condition) nodeName = "条件";
		else if (editor_node.type == EditorNodeType::PersistentSelector) nodeName = "永続 選択";
		else if (editor_node.type == EditorNodeType::PersistentSequence) nodeName = "永続 シーケンス";
		else if (editor_node.type == EditorNodeType::RestartingSelector) nodeName = "再起動 選択";
		else if (editor_node.type == EditorNodeType::RestartingSequence) nodeName = "再起動 シーケンス";

		runtime_node->SetDebugInfo(
			editor_node.id,
			editor_node.inputPinId,
			editor_node.outputPinId,
			editor_node.pos,
			nodeName,
			editor_node.type
		);
	}


	return runtime_node;
}