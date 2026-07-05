#include "ComboTreeFactory.h"
#include <unordered_set>
#include "GrowthEngine.h"
#include "../ComboTreeData/ComboTreeData.h"

using json = nlohmann::json;

/// @brief コンボツリーを生成する
/// @param jsonFilePath 
/// @param character 
/// @return 
ComboTree ComboTreeFactory::CreateTree(const std::string& jsonFilePath, Character* character)
{
    ComboTree tree;
    std::ifstream file(jsonFilePath);
    if (!file.is_open()) return tree; // 読み込み失敗

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
		int outputLightPinId = nodeJson.value("outputLightPinId", 0);
		int outputHeavyPinId = nodeJson.value("outputHeavyPinId", 0);
		std::string animName = nodeJson.value("animationName", "");
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
            tree.allAttacks.push_back(std::move(attack));
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
            tree.allAttacks.push_back(std::move(attack));
        }

        // 検索用マップに登録
        nodeMap[nodeId] = attackPtr;

        pinToNodeMap[inputPinId] = nodeId;

        pinToNodeMap[outputLightPinId] = nodeId;
        pinToTypeMap[outputLightPinId] = AttackInputType::Light; // このピンは「弱」

        pinToNodeMap[outputHeavyPinId] = nodeId;
        pinToTypeMap[outputHeavyPinId] = AttackInputType::Heavy; // このピンは「強」
    }

	// リンク情報を読み込み、ノード間の接続を構築
    for (const auto& linkJson : rootJson["links"])
    {
        int startPinId = linkJson["startPinId"];
        int endPinId = linkJson["endPinId"];

        int startNodeId = pinToNodeMap[startPinId];
        int endNodeId = pinToNodeMap[endPinId];

        // 繋ぎ元（親）は必ず ComboAttack なのでキャスト
        ComboAttack* parentAttack = dynamic_cast<ComboAttack*>(nodeMap[startNodeId]);

        // 繋ぎ先（子）は ComboAttack または GrabAttack (基底の Attack*)
        Attack* childAttack = nodeMap[endNodeId];

		// 親ノードの次の攻撃として子ノードを設定
        if (parentAttack && childAttack)
        {
            if (pinToTypeMap[startPinId] == AttackInputType::Light)
            {
                parentAttack->SetNextLightAttack(childAttack);
            }
            else if (pinToTypeMap[startPinId] == AttackInputType::Heavy)
            {
                parentAttack->SetNextHeavyAttack(childAttack);
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
            // ルートになるのは基本ComboAttackなのでキャストしておく（ツリー構造の定義に合わせます）
            tree.rootAttack = dynamic_cast<ComboAttack*>(pair.second);
            break;
        }
    }

    return tree;
}