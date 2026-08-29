#include "ComboTreeFactory.h"
#include <unordered_set>
#include "GrowthEngine.h"

/// @brief コンボツリーを生成する（エディタ上のノードとリンクから）
/// @param editorNodes 
/// @param editorLinks 
/// @param character 
/// @return 
std::unique_ptr<ComboTree> ComboTreeFactory::CreateTree(const std::vector<ComboEditorNode>& editorNodes, const std::vector<ComboEditorLink>& editorLinks,
	Character* character)
{
	// ノードが何もない場合は作成しない
	if (editorNodes.empty())
		return nullptr;

	// コンボツリーのインスタンスを生成
	std::unique_ptr<ComboTree> tree = std::make_unique<ComboTree>();

	// 一時的な検索用マップ
	std::unordered_map<int, Attack*> nodeMap; // ノードID -> インスタンスポインタ
	std::unordered_map<int, int> pinToNodeMap; // ピンID -> そのピンを持つノードID
	std::unordered_map<int, AttackInputType> pinToTypeMap; // 出力ピンID -> 弱か強か

	// どのノードが「入力」を受けたかを記録するセット（ルートノード特定用）
	std::unordered_set<int> hasInputNodes;


	// ノード情報を読み込み、ComboAttackインスタンスを生成
	for (const auto& node : editorNodes)
	{
		Attack* attackPtr = nullptr;

		// ノードの種類に応じて攻撃を生成
		if (node.nodeType == ComboNodeType::Combo)
		{
			CombAttackInitData initData = node.comboAttackInitData;

			// アニメーションをセット
			if (!node.motionName.empty())
			{
				initData.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, node.motionName);
			}

			auto attack = std::make_unique<ComboAttack>(character, initData);
			attackPtr = attack.get();
			tree->AddAttack(std::move(attack));
		}
		else if (node.nodeType == ComboNodeType::Grab)
		{
			GrabAttackInitData initData = node.grabAttackInitData;

			// アニメーションをセット
			if (!node.motionName.empty())
			{
				initData.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, node.motionName);
			}

			auto attack = std::make_unique<GrabAttack>(character, initData);
			attackPtr = attack.get();
			tree->AddAttack(std::move(attack));
		}
		else if (node.nodeType == ComboNodeType::GrabStrike)
		{
			GrabStrikeAttackInitData initData = node.grabStrikeAttackInitData;

			// アニメーションをセット
			if (!node.motionName.empty())
			{
				initData.hAttackAnimation = MotionManager::GetInstance()->GetMotion(MotionType::Attack, node.motionName);
			}

			auto attack = std::make_unique<GrabStrikeAttack>(character, initData);
			attackPtr = attack.get();
			tree->AddAttack(std::move(attack));
		}

		// 検索用マップに登録
		nodeMap[node.id] = attackPtr;

		pinToNodeMap[node.inputPinId] = node.id;

		pinToNodeMap[node.outputInputXPinId] = node.id;
		pinToTypeMap[node.outputInputXPinId] = AttackInputType::InputX; // このピンはX

		pinToNodeMap[node.outputInputYPinId] = node.id;
		pinToTypeMap[node.outputInputYPinId] = AttackInputType::InputY; // このピンはY

		pinToNodeMap[node.outputInputBPinId] = node.id;
		pinToTypeMap[node.outputInputBPinId] = AttackInputType::InputB; // このピンはB
	}

	// リンク情報を読み込み、ノード間の接続を構築
	for (const auto& link : editorLinks)
	{
		int startNodeId = pinToNodeMap[link.startPinId];
		int endNodeId = pinToNodeMap[link.endPinId];

		// 繋ぎ元（親）は必ず ComboAttack なのでキャスト
		Attack* parentAttack = nodeMap[startNodeId];

		// 繋ぎ先（子）は ComboAttack または GrabAttack (基底の Attack*)
		Attack* childAttack = nodeMap[endNodeId];

		// 親ノードの次の攻撃として子ノードを設定
		if (parentAttack && childAttack)
		{
			if (pinToTypeMap[link.startPinId] == AttackInputType::InputX)
			{
				parentAttack->SetNextInputXAttack(childAttack);
			}
			else if (pinToTypeMap[link.startPinId] == AttackInputType::InputY)
			{
				parentAttack->SetNextInputYAttack(childAttack);
			}
			else if (pinToTypeMap[link.startPinId] == AttackInputType::InputB)
			{
				parentAttack->SetNextInputBAttack(childAttack);
			}
		}

		// リンク先（子）のノードは、ルート（始点）ではないことを記録
		hasInputNodes.insert(endNodeId);
	}

	// ルートノードを特定する
	for (const auto& pair : nodeMap)
	{
		int nodeId = pair.first;
		if (hasInputNodes.find(nodeId) == hasInputNodes.end())
		{
			tree->SetRootAttack(pair.second);
			break;
		}
	}

	return tree;
}