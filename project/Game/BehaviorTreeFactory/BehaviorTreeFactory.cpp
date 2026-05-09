#include "BehaviorTreeFactory.h"
#include "Node/ConditionNode/ConditionNode.h"
#include "Node/CompositeNode/PersistentSelectorNode/PersistentSelectorNode.h"
#include "Node/CompositeNode/PersistentSequenceNode/PersistentSequenceNode.h"
#include "Node/CompositeNode/RestartingSelectorNode/RestartingSelectorNode.h"
#include "Node/CompositeNode/RestartingSequenceNode/RestartingSequenceNode.h"
#include "Node/ActionNode/ComboAttackNode/ComboAttackNode.h"
#include "Node/ActionNode/GrabAttackNode/GrabAttackNode.h"

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
        // 実際は editor_node.condition_name 等をもとに、
        // 登録済みの関数辞書から std::function を引いてきてバインドします
        runtime_node = std::make_unique<ConditionNode>([]() { return true; /* 仮の処理 */ });
        break;

    case EditorNodeType::Action:

        // エディターで設定した文字列（actionName）に応じて生成するノードを変える
        if (editor_node.actionName == "ComboAttack")
        {
            // ※本来は JSON や外部データから攻撃パラメータを読み込むのが理想です
            CombAttackInitData initData;
            // initData.attackTime = editor_node.customFloat; のように
            // EditorNodeに持たせたパラメータをここで代入することも可能です

            auto comboAction = std::make_unique<ComboAttack>(character, initData);
            runtime_node = std::make_unique<ComboAttackNode>(std::move(comboAction));
        }
        else if (editor_node.actionName == "GrabAttack")
        {
            GrabAttackInitData initData;
            auto grabAction = std::make_unique<GrabAttack>(character, initData);
            runtime_node = std::make_unique<GrabAttackNode>(std::move(grabAction));
        }
        else
        {
            // 何も設定されていない、または該当しない場合（何もしないノードにする等）
            // runtime_node = nullptr;
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
                ImVec2 pos_a = ImNodes::GetNodeGridSpacePos(a->id);
                ImVec2 pos_b = ImNodes::GetNodeGridSpacePos(b->id);
                return pos_a.y < pos_b.y;
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

    return runtime_node;
}