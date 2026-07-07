#include "ComboTreeFactory.h"
#include <unordered_set>
#include "GrowthEngine.h"
#include "../ComboTreeData/ComboTreeData.h"

using json = nlohmann::json;

/// @brief コンボツリーを生成する
/// @param jsonFilePath 
/// @param character 
/// @return 
std::unique_ptr<ComboTree> ComboTreeFactory::CreateTree(const std::string& jsonFilePath, Character* character)
{
	std::string directoryPath = "./Assets/Parameter/ComboTree/" + jsonFilePath;

	std::unique_ptr<ComboTree> tree = std::make_unique<ComboTree>();
    std::ifstream file(directoryPath);
    if (!file.is_open()) return nullptr; // 読み込み失敗

    json rootJson;
    file >> rootJson;

    // 一時的な検索用マップ
    std::unordered_map<int, Attack*> nodeMap;           // ノードID -> インスタンスポインタ
    std::unordered_map<int, int> pinToNodeMap;               // ピンID -> そのピンを持つノードID
    std::unordered_map<int, AttackInputType> pinToTypeMap;   // 出力ピンID -> 弱か強か

    // どのノードが「入力」を受けたかを記録するセット（ルートノード特定用）
    std::unordered_set<int> hasInputNodes;

	// ノード情報を読み込み、ComboAttackインスタンスを生成
    for (const auto& nodeJson : rootJson["nodes"])
    {
        int nodeId = nodeJson.value("id", 0);
		int inputPinId = nodeJson.value("inputPinId", 0);
		int outputInputXPinId = nodeJson.value("outputInputXPinId", 0);
		int outputInputYPinId = nodeJson.value("outputInputYPinId", 0);
		int outputInputBPinId = nodeJson.value("outputInputBPinId", 0);
		std::string animName = nodeJson.value("animationName", "");
		std::string targetAnimName = nodeJson.value("targetAnimationName", "");
		ComboNodeType nodeType = static_cast<ComboNodeType>(nodeJson.value("nodeType", 0));
        Attack* attackPtr = nullptr;

		// ノードのパラメータをJSONから取得
		json paramsJson;

		if (nodeType == ComboNodeType::Combo && nodeJson.contains("comboParams"))
        {
			// ComboAttackのパラメータを取得
			paramsJson = nodeJson["comboParams"];

            // InitDataをJSONから構築
            CombAttackInitData initData;

            if (!animName.empty())
            {
                // ※ プロジェクトの MotionManager の仕様に合わせて関数名は適宜調整してください
                initData.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, animName);
            }

            initData.attackTime = paramsJson["attackTime"];
            initData.moveSpeed = paramsJson["moveSpeed"];
            initData.moveStartTime = paramsJson["moveStartTime"];
            initData.moveEndTime = paramsJson["moveEndTime"];
            initData.cancelStartTime = paramsJson["cancelStartTime"];
            initData.cancelEndTime = paramsJson["cancelEndTime"];

            // 当たり判定の読み込み
            if (paramsJson.contains("hitDefinitions"))
            {
                for (const auto& defJson : paramsJson["hitDefinitions"])
                {
                    HitboxDefinition def;
                    def.startTime = defJson["startTime"];
                    def.endTime = defJson["endTime"];
                    def.damage = defJson["damage"];
                    def.radius = defJson["radius"];
                    def.knockback = defJson["knockback"];
                    auto dir = defJson["knockbackDirection"];
                    def.knockbackDirection = Vector3(dir[0], dir[1], dir[2]);
                    def.damageReaction = static_cast<DamageReaction>(defJson["damageReaction"].get<int>());
					def.jointType = static_cast<JointType>(defJson["jointType"].get<int>());

                    initData.hitDefinitions.push_back(def);
                }
            }

            // ComboAttackインスタンスを生成してリストに保管
            auto attack = std::make_unique<ComboAttack>(character, initData);
            attackPtr = attack.get();
			tree->AddAttack(std::move(attack));
        }
        else if (nodeType == ComboNodeType::Grab && nodeJson.contains("grabParams"))
        {
			// GrabAttackのパラメータを取得
			paramsJson = nodeJson["grabParams"];

			// InitDataをJSONから構築
			GrabAttackInitData initData;

            if (!animName.empty())
            {
                // ※ プロジェクトの MotionManager の仕様に合わせて関数名は適宜調整してください
                initData.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, animName);
            }

			initData.attackTime = paramsJson.value("attackTime", 0.0f);
			initData.moveSpeed = paramsJson.value("moveSpeed", 0.0f);
			initData.moveStartTime = paramsJson.value("moveStartTime", 0.0f);
			initData.moveEndTime = paramsJson.value("moveEndTime", 0.0f);
			initData.grabTime = paramsJson.value("grabTime", 0.0f);

			initData.hitboxStartTime = paramsJson.value("hitboxStartTime", 0.0f);
			initData.hitboxEndTime = paramsJson.value("hitboxEndTime", 0.0f);
			initData.jointType = static_cast<JointType>(paramsJson.value("jointType", 0));

            // ComboAttackインスタンスを生成してリストに保管
            auto attack = std::make_unique<GrabAttack>(character, initData);
            attackPtr = attack.get();
			tree->AddAttack(std::move(attack));
        }
        else if (nodeType == ComboNodeType::GrabStrike && nodeJson.contains("grabStrikeParams"))
        {
			// GrabStrikeAttackのパラメータを取得
			paramsJson = nodeJson["grabStrikeParams"];

			GrabStrikeAttackInitData initData;

			if (!animName.empty())
			{
				// ※ プロジェクトの MotionManager の仕様に合わせて関数名は適宜調整してください
				initData.hAttackAnimation = MotionManager::GetInstance()->GetMotion(MotionType::Attack, animName);
			}

			if (!targetAnimName.empty())
			{
				initData.hTargetAnimation = MotionManager::GetInstance()->GetMotion(MotionType::Attack, targetAnimName);
			}

			initData.attackTime = paramsJson.value("attackTime", 0.0f);
			initData.moveSpeed = paramsJson.value("moveSpeed", 0.0f);
			initData.moveStartTime = paramsJson.value("moveStartTime", 0.0f);
			initData.moveEndTime = paramsJson.value("moveEndTime", 0.0f);
			initData.knockback = paramsJson.value("knockback", 0.0f);
			initData.knockbackDirection = Vector3(
				paramsJson.value("knockbackDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[0],
				paramsJson.value("knockbackDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[1],
				paramsJson.value("knockbackDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[2]
			);

			initData.isRelease = paramsJson.value("isRelease", false);
			initData.releaseTime = paramsJson.value("releaseTime", 0.0f);
			initData.damageReaction = static_cast<DamageReaction>(paramsJson.value("damageReaction", 0));

            if (paramsJson.contains("hitDefinitions"))
            {
                for (const auto& defJson : paramsJson["hitDefinitions"])
                {
                    HitDefinition def;
					def.hitTime = defJson["hitTime"];
                    def.damage = defJson["damage"];
					def.hitJoint = static_cast<JointType>(defJson["hitJoint"].get<int>());

                    initData.hits.push_back(def);
                }
            }
            
			auto attack = std::make_unique<GrabStrikeAttack>(character, initData);
			attackPtr = attack.get();
			tree->AddAttack(std::move(attack));
        }

        // 検索用マップに登録
        nodeMap[nodeId] = attackPtr;

        pinToNodeMap[inputPinId] = nodeId;

        pinToNodeMap[outputInputXPinId] = nodeId;
        pinToTypeMap[outputInputXPinId] = AttackInputType::InputX; // このピンはX

        pinToNodeMap[outputInputYPinId] = nodeId;
        pinToTypeMap[outputInputYPinId] = AttackInputType::InputY; // このピンはY

		pinToNodeMap[outputInputBPinId] = nodeId;
		pinToTypeMap[outputInputBPinId] = AttackInputType::InputB; // このピンはB
    }

	// リンク情報を読み込み、ノード間の接続を構築
    for (const auto& linkJson : rootJson["links"])
    {
        int startPinId = linkJson["startPinId"];
        int endPinId = linkJson["endPinId"];

        int startNodeId = pinToNodeMap[startPinId];
        int endNodeId = pinToNodeMap[endPinId];

        // 繋ぎ元（親）は必ず ComboAttack なのでキャスト
        Attack* parentAttack = nodeMap[startNodeId];

        // 繋ぎ先（子）は ComboAttack または GrabAttack (基底の Attack*)
        Attack* childAttack = nodeMap[endNodeId];

		// 親ノードの次の攻撃として子ノードを設定
        if (parentAttack && childAttack)
        {
            if (pinToTypeMap[startPinId] == AttackInputType::InputX)
            {
                parentAttack->SetNextInputXAttack(childAttack);
            }
            else if (pinToTypeMap[startPinId] == AttackInputType::InputY)
            {
                parentAttack->SetNextInputYAttack(childAttack);
            }
			else if (pinToTypeMap[startPinId] == AttackInputType::InputB)
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