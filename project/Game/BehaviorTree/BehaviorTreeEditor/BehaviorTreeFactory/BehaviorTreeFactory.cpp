#include "BehaviorTreeFactory.h"
#include "Entity/Character/Character.h"
#include "BattleDirector/BattleDirector.h"
#include "Node/ConditionNode/ConditionNode.h"
#include "Node/CompositeNode/PersistentSelectorNode/PersistentSelectorNode.h"
#include "Node/CompositeNode/PersistentSequenceNode/PersistentSequenceNode.h"
#include "Node/CompositeNode/RestartingSelectorNode/RestartingSelectorNode.h"
#include "Node/CompositeNode/RestartingSequenceNode/RestartingSequenceNode.h"
#include "Node/CompositeNode/UtilitySelectorNode/UtilitySelectorNode.h"
#include "Node/CompositeNode/WeightedRandomSelectorNode/WeightedRandomSelectorNode.h"
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
std::unique_ptr<BehaviorTree> BehaviorTreeFactory::CreateTree(const std::vector<EditorNode>& editorNodes, const std::vector<EditorLink>& editorLinks, 
	Character* character, const std::string& name)
{
	// ルートノードを見つける
	const EditorNode* rootEditorNode = FindRootNode(editorNodes, editorLinks);

	// ルートノードが見つからない場合は nullptr を返す
	if (!rootEditorNode) return nullptr;

	// ルートノードから再帰的にランタイムノードを構築する
	std::unique_ptr<Node> rootRuntimeNode = BuildNodeRecursive(*rootEditorNode, editorNodes, editorLinks, character);

	// ルートノードが構築できなかった場合は nullptr を返す
	return std::make_unique<BehaviorTree>(std::move(rootRuntimeNode), name);
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
std::unique_ptr<Node> BehaviorTreeFactory::BuildNodeRecursive(const EditorNode& editorNode, const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links, Character* character)
{
	// サブツリーノードの場合は、サブツリーのファイルを読み込んで再帰的にツリーを生成する
	if (editorNode.type == EditorNodeType::SubTree)
	{
		// サブツリーのファイルを読み込んで再帰的にツリーを生成する
		std::vector<EditorNode> subNodes;
		std::vector<EditorLink> subLinks;

		BehaviorTreeSetting setting("BehaviorTree");
		setting.LoadTree(editorNode.subTreeFileName, subNodes, subLinks);

		// ロードしたサブツリーのルートノードからツリーインスタンスを構築して返す
		return BuildNodeRecursive(*FindRootNode(subNodes, subLinks), subNodes, subLinks, character);
	}


	// editor_node の種類に応じて対応するランタイムノードを生成する
	std::unique_ptr<Node> runtimeNode = nullptr;

	// 条件関数の宣言（条件ノードの場合に使用）
	std::function<bool(Character*)> conditionFunc{};

	// ユーティリティ関数の宣言（ユーティリティノードの場合に使用）
	std::function<float(Character*)> utilityFunc{};

	// ノードの種類に応じて対応するランタイムノードを生成
	if (editorNode.type == EditorNodeType::RestartingSelector)
	{
		runtimeNode = std::make_unique<RestartingSelectorNode>();
	}
	else if (editorNode.type == EditorNodeType::PersistentSelector)
	{
		runtimeNode = std::make_unique<PersistentSelectorNode>();
	}
	else if (editorNode.type == EditorNodeType::RestartingSequence)
	{
		runtimeNode = std::make_unique<RestartingSequenceNode>();
	}
	else if (editorNode.type == EditorNodeType::PersistentSequence)
	{
		runtimeNode = std::make_unique<PersistentSequenceNode>();
	}
	else if (editorNode.type == EditorNodeType::UtilitySelector)
	{
		// UtilitySelectorNode本体の生成
		auto utilitySelector = std::make_unique<UtilitySelectorNode>(character);

		// 子ノードのIDを取得
		std::vector<int> childIds = GetChildNodeIds(nodes, links, editorNode.id);

		// 子ノードごとに再帰的な生成と評価関数の紐づけを行う
		for (int childId : childIds)
		{
			const EditorNode& childEditorNode = GetEditorNode(nodes, childId);
			std::unique_ptr<Node> childRuntimeNode = BuildNodeRecursive(childEditorNode, nodes, links, character);

			// 子ノードのIDに対応する評価タイプを取得
			UtilityType uType = UtilityType::FixedDefault;
			if (editorNode.childUtilityMap.find(childId) != editorNode.childUtilityMap.end())
			{
				uType = editorNode.childUtilityMap.at(childId);
			}

			// 評価タイプに応じてラムダ式（評価関数）を生成
			std::function<float(Character*)> utilityFunc;
			switch (uType)
			{
				// 体力比率を評価する関数
			case UtilityType::HpRatio:
				utilityFunc = [character](Character* owner) ->float { return static_cast<float>(owner->GetHp()) / static_cast<float>(owner->GetMaxHp()); };
				break;

				// 固定値を返す関数（デフォルト）
			case UtilityType::FixedDefault:
			default:
				utilityFunc = [character](Character* owner) ->float { return 0.5f; };
				break;
			}

			// 生成した子ノードと評価関数をセットにして追加
			utilitySelector->AddChildWithUtility(std::move(childRuntimeNode), utilityFunc);
		}

		runtimeNode = std::move(utilitySelector);
	}
	else if (editorNode.type == EditorNodeType::WeightedRandomSelector)
	{
		// WeightedRandomSelectorNode本体の生成
		auto weightedRandomSelector = std::make_unique<WeightedRandomSelectorNode>();

		// 子ノードのIDを取得
		std::vector<int> childIds = GetChildNodeIds(nodes, links, editorNode.id);

		for (int childId : childIds)
		{
			// 子ノードのエディタノードを取得
			const EditorNode& childEditorNode = GetEditorNode(nodes, childId);

			// 子ノードを再帰的に生成
			std::unique_ptr<Node> childRuntimeNode = BuildNodeRecursive(childEditorNode, nodes, links, character);

			// 子ノードの重みを取得（デフォルトは1.0f）
			float weight = 1.0f;
			if (editorNode.childWeightMap.find(childId) != editorNode.childWeightMap.end())
			{
				weight = editorNode.childWeightMap.at(childId);
			}

			// 生成した子ノードと重みをセットにして追加
			weightedRandomSelector->AddChildWithWeight(std::move(childRuntimeNode), weight);
		}
		
		runtimeNode = std::move(weightedRandomSelector);
	}
	else if (editorNode.type == EditorNodeType::Condition)
	{
		// エディタで設定した条件の種類に応じて、条件関数を生成する
		switch (editorNode.conditionType)
		{
			// 関数なし、常に true を返す条件
		case ConditionType::None:
		default:
			conditionFunc = [](Character* character) { return true; };
			break;

			// ターゲットがいるかどうかをチェックする条件
		case ConditionType::HasTarget:
			conditionFunc = [](Character* character) { return character->HasTarget(); };
			break;

			// ターゲットがダウンしているかどうかをチェックする条件
		case ConditionType::IsTargetDown:
			conditionFunc = [](Character* character) { return character->HasTarget() && character->GetLockOnTarget()->IsDown(); };
			break;

			// ターゲットがダウンしていないかどうかをチェックする条件
		case ConditionType::IsNotTargetDown:
			conditionFunc = [](Character* character) { return !character->HasTarget() || !character->GetLockOnTarget()->IsDown(); };
			break;

			// 掴んでいるかどうかをチェックする条件
		case ConditionType::IsGrabbing:
			conditionFunc = [](Character* character) { return character->IsGrabbing(); };
			break;

			// 掴んでいないかどうかをチェックする条件
		case ConditionType::IsNotGrabbing:
			conditionFunc = [](Character* character) { return !character->IsGrabbing(); };
			break;

			// ターゲットが一定距離内にいるかどうかをチェックする条件
		case ConditionType::IsTargetInRange:
			conditionFunc = [editorNode](Character* character)
				{
					if (!character->HasTarget()) return false;
					float distance = (character->GetLockOnTarget()->GetWorldPosition() - character->GetWorldPosition()).Length();
					return distance <= editorNode.conditionParam.distanceToTarget;
				};
			break;

			// ターゲットが一定距離外にいるかどうかをチェックする条件
		case ConditionType::IsTargetOutOfRange:
			conditionFunc = [editorNode](Character* character)
				{
					if (!character->HasTarget()) return true;
					float distance = (character->GetLockOnTarget()->GetWorldPosition() - character->GetWorldPosition()).Length();
					return distance > editorNode.conditionParam.distanceToTarget;
				};
			break;

			// ターゲットが攻撃しているかどうかをチェックする条件
		case ConditionType::IsTargetAttacking:
			conditionFunc = [](Character* character)
				{
					if (!character->HasTarget()) return false;
					return character->GetLockOnTarget()->IsAttack();
				};
			break;

			// ターゲットが攻撃していないかどうかをチェックする条件
		case ConditionType::IsTargetNotAttacking:
			conditionFunc = [](Character* character)
				{
					if (!character->HasTarget()) return true;
					return !character->GetLockOnTarget()->IsAttack();
				};
			break;

			// ターゲットが攻撃動作中かどうかをチェックする条件
		case ConditionType::IsTargetInAttackSequence:
			conditionFunc = [](Character* character)
				{
					if (!character->HasTarget()) return false;
					return character->GetLockOnTarget()->IsInAttackSequence();
				};
			break;

			// ターゲットが攻撃動作中でないかどうかをチェックする条件
		case ConditionType::IsTargetNotInAttackSequence:
			conditionFunc = [](Character* character)
				{
					if (!character->HasTarget()) return true;
					return !character->GetLockOnTarget()->IsInAttackSequence();
				};
			break;

			// 攻撃動作中かどうかをチェックする条件
		case ConditionType::IsInAttackSequence:
			conditionFunc = [](Character* character) { return character->IsInAttackSequence(); };
			break;

			// 攻撃動作中でないかどうかをチェックする条件
		case ConditionType::IsNotInAttackSequence:
			conditionFunc = [](Character* character) { return !character->IsInAttackSequence(); };
			break;

			// 回避動作中かどうかをチェックする条件
		case ConditionType::IsAvoiding:
			conditionFunc = [](Character* character) { return character->IsAvoid(); };
			break;

			// 回避動作中でないかどうかをチェックする条件
		case ConditionType::IsNotAvoiding:
			conditionFunc = [](Character* character) { return !character->IsAvoid(); };
			break;

		case ConditionType::IsDamageReaction:
			conditionFunc = [](Character* character) { return character->IsDamageReaction(); };
			break;

		case ConditionType::IsNotDamageReaction:
			conditionFunc = [](Character* character) { return !character->IsDamageReaction(); };
			break;

		case ConditionType::IsChangeState:
			conditionFunc = [](Character* character) { return character->GetStateMachine()->IsChangeState(); };
			break;

		case ConditionType::IsNotChangeState:
			conditionFunc = [](Character* character) { return !character->GetStateMachine()->IsChangeState(); };
			break;
		}

		// 条件関数を使用して条件ノードを生成
		runtimeNode = std::make_unique<ConditionNode>(character, conditionFunc);
	}
	else if(editorNode.type == EditorNodeType::Action)
	{
		// エディターで設定した文字列（actionName）に応じて生成するノードを変える
		if (editorNode.actionType == ActionType::ComboAttack)
		{
			// エディタ上で設定した初期化データを使用
			CombAttackInitData initData = editorNode.comboAttackInitData;

			auto comboAction = std::make_unique<ComboAttack>(character, initData);
			runtimeNode = std::make_unique<ComboAttackNode>(std::move(comboAction));
		}
		else if (editorNode.actionType == ActionType::GrabAttack)
		{
			// エディタ上で設定した初期化データを使用
			GrabAttackInitData initData = editorNode.grabAttackInitData;

			auto grabAction = std::make_unique<GrabAttack>(character, initData);
			runtimeNode = std::make_unique<GrabAttackNode>(std::move(grabAction));
		}
		else if (editorNode.actionType == ActionType::GrabStrikeAttack)
		{
			// エディタ上で設定した初期化データを使用
			GrabStrikeAttackInitData initData = editorNode.grabStrikeAttackInitData;
			auto grabStrikeAction = std::make_unique<GrabStrikeAttack>(character, initData);
			runtimeNode = std::make_unique<GrabStrikeAttackNode>(std::move(grabStrikeAction));
		}
		else if (editorNode.actionType == ActionType::RequestToken)
		{
			// トークン要求ノードの生成
			runtimeNode = std::make_unique<RequestTokenNode>(std::make_unique<RequestToken>(character, editorNode.tokenType));
		}
		else if (editorNode.actionType == ActionType::ReleaseToken)
		{
			// トークン解放ノードの生成
			runtimeNode = std::make_unique<ReleaseTokenNode>(std::make_unique<ReleaseToken>(character));
		}
		else if (editorNode.actionType == ActionType::Avoid)
		{
			// 回避ノードの生成
			runtimeNode = std::make_unique<ActionNode>(std::make_unique<Avoid>(character, editorNode.avoidInitData));
		}
		else if (editorNode.actionType == ActionType::NavMeshMove)
		{
			// NavMeshを使用した移動ノードの生成
			runtimeNode = std::make_unique<ActionNode>(std::make_unique<NavMeshMove>(character, editorNode.navMeshMoveInitData));
		}
		else if (editorNode.actionType == ActionType::NavMeshLeaderMove)
		{
			// NavMeshを使用したリーダーへの移動ノードの生成
			runtimeNode = std::make_unique<ActionNode>(std::make_unique<NavMeshLeaderMove>(character, editorNode.navMeshLeaderMoveInitData));
		}
		else if (editorNode.actionType == ActionType::InAttackSequence)
		{
			// 攻撃シーケンス開始ノードの生成
			runtimeNode = std::make_unique<InAttackSequenceNode>(std::make_unique<InAttackSequence>(character));
		}
		else if (editorNode.actionType == ActionType::OutAttackSequence)
		{
			// 攻撃シーケンス終了ノードの生成
			runtimeNode = std::make_unique<OutAttackSequenceNode>(std::make_unique<OutAttackSequence>(character));
		}
		else if (editorNode.actionType == ActionType::Telegraph)
		{
			// 予備動作ノードの生成
			runtimeNode = std::make_unique<ActionNode>(std::make_unique<Telegraph>(character, editorNode.telegraphInitData));
		}
		else if (editorNode.actionType == ActionType::Defense)
		{
			// 防御ノードの生成
			runtimeNode = std::make_unique<ActionNode>(std::make_unique<Defense>(character, editorNode.defenseInitData));
		}
		else
		{
			// 何も設定されていない、または該当しない場合（何もしないノードにする等）
			runtimeNode = nullptr;
		}
	}

	// ランタイムノードが生成できなかった場合は nullptr を返す
	if (auto composite_node = dynamic_cast<CompositeNode*>(runtimeNode.get()))
	{
		if (editorNode.type != EditorNodeType::UtilitySelector &&
			editorNode.type != EditorNodeType::WeightedRandomSelector)
		{
			std::vector<const EditorNode*> child_editor_nodes;

			// editorNode の出力ピンに接続されているリンクを探す
			for (const auto& link : links)
			{
				if (link.startPinId == editorNode.outputPinId)
				{
					// リンクの終了ピンに対応するノードを探す
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
				[](const EditorNode* a, const EditorNode* b)
				{
					// ImNodesの関数を使わず、ロード済みの EditorNode のデータ(pos)を直接比較する
					return a->pos.y < b->pos.y;
				}
			);

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
	}


#ifdef DEVELOPMENT

	// ラインタイムでノードを確認するためのデバッグ情報を設定
	if (runtimeNode)
	{
		std::string nodeName = "";

		// エディタ側でユーザーが名前を入力しているかチェック
		if (editorNode.name[0] != '\0')
		{
			nodeName = editorNode.name;
		}
		else
		{
			// 空欄だった場合は、これまで通りノードのタイプに応じたデフォルト名を設定
			switch (editorNode.type)
			{
			case EditorNodeType::PersistentSelector: nodeName = "永続 セレクタ"; break;
			case EditorNodeType::PersistentSequence: nodeName = "永続 シーケンス"; break;
			case EditorNodeType::RestartingSelector: nodeName = "再起動 セレクタ"; break;
			case EditorNodeType::RestartingSequence: nodeName = "再起動 シーケンス"; break;
			case EditorNodeType::UtilitySelector:    nodeName = "ユーティリティ セレクタ"; break;
			case EditorNodeType::WeightedRandomSelector: nodeName = "重み付きランダム セレクタ"; break;
			case EditorNodeType::Condition:          nodeName = "条件"; break;
			case EditorNodeType::Action:             nodeName = "アクション"; break;
			default:                                 nodeName = "未知のノード"; break;
			}
		}

		// 決定した名前をランタイムノードのデバッグ情報としてセット
		runtimeNode->SetDebugInfo(editorNode.id, editorNode.inputPinId, editorNode.outputPinId, editorNode.pos, nodeName, editorNode.type);
	}

#endif


	return runtimeNode;
}

/// @brief 指定された親ノードIDに対応する子ノードのIDを取得する
/// @param links 
/// @param parentId 
/// @return 
std::vector<int> BehaviorTreeFactory::GetChildNodeIds(const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links, int parentId)
{
	std::vector<int> childIds;

	// 全リンクの中から、出発点が parentId と一致するものを探す
	for (const auto& link : links)
	{
		if (link.startNodeId == parentId)
		{
			childIds.push_back(link.endNodeId);
		}
	}

	// 子ノードのIDをY座標でソートして、エディタ上の見た目の順番で実行されるようにする
	std::sort(childIds.begin(), childIds.end(), [&nodes](int a, int b) 
		{
			const EditorNode& nodeA = GetEditorNode(nodes, a);
			const EditorNode& nodeB = GetEditorNode(nodes, b);
			return nodeA.pos.y < nodeB.pos.y;
		}
	);

	return childIds;
}

/// @brief 指定されたノードIDに対応するEditorNodeを取得する
/// @param nodes 
/// @param nodeId 
/// @return 
const EditorNode& BehaviorTreeFactory::GetEditorNode(const std::vector<EditorNode>& nodes, int nodeId)
{
	// ノードIDに一致するEditorNodeを検索
	for (const auto& node : nodes)
	{
		if (node.id == nodeId)
		{
			return node;
		}
	}

	// 一致するノードが見つからなかった場合は例外を投げる
	throw std::runtime_error("EditorNode not found. ID: " + std::to_string(nodeId));
}