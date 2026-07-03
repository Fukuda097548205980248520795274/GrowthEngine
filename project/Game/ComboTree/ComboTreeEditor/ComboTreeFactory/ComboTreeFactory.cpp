#include "ComboTreeFactory.h"
#include <unordered_set>
#include "GrowthEngine.h"

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
    std::unordered_map<int, ComboAttack*> nodeMap;           // ノードID -> インスタンスポインタ
    std::unordered_map<int, int> pinToNodeMap;               // ピンID -> そのピンを持つノードID
    std::unordered_map<int, AttackInputType> pinToTypeMap;   // 出力ピンID -> 弱か強か

    // どのノードが「入力」を受けたかを記録するセット（ルートノード特定用）
    std::unordered_set<int> hasInputNodes;

	// ノード情報を読み込み、ComboAttackインスタンスを生成
    for (const auto& nodeJson : rootJson["nodes"])
    {
        int nodeId = nodeJson["id"];
        int inputPinId = nodeJson["inputPinId"];
        int outputLightPinId = nodeJson["outputLightPinId"];
        int outputHeavyPinId = nodeJson["outputHeavyPinId"];

        // InitDataをJSONから構築
        CombAttackInitData initData;

        // ※ 実際はここで animationName から AnimationHandle への変換処理を入れます
        // initData.hAttackMotion = MotionManager::GetInstance()->LoadAnimation(nodeJson["animationName"]);

        initData.attackTime = nodeJson["attackTime"];
        initData.moveSpeed = nodeJson["moveSpeed"];
        initData.moveStartTime = nodeJson["moveStartTime"];
        initData.moveEndTime = nodeJson["moveEndTime"];
        initData.cancelStartTime = nodeJson["cancelStartTime"];
        initData.cancelEndTime = nodeJson["cancelEndTime"];

        // 当たり判定の読み込み
        if (nodeJson.contains("hitDefinitions"))
        {
            for (const auto& defJson : nodeJson["hitDefinitions"])
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

                initData.hitDefinitions.push_back(def);
            }
        }

        // ComboAttackインスタンスを生成してリストに保管
        auto attack = std::make_unique<ComboAttack>(character, initData);
        ComboAttack* attackPtr = attack.get();
        tree.allAttacks.push_back(std::move(attack));

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

        ComboAttack* parentAttack = nodeMap[startNodeId];
        ComboAttack* childAttack = nodeMap[endNodeId];

        // 弱ピンから出た線なら、親のNextLightにセット。強ならNextHeavyにセット。
        if (pinToTypeMap[startPinId] == AttackInputType::Light)
        {
            parentAttack->SetNextLightAttack(childAttack);
        }
        else if (pinToTypeMap[startPinId] == AttackInputType::Heavy)
        {
            parentAttack->SetNextHeavyAttack(childAttack);
        }

        // リンク先（子）のノードは、ルート（始点）ではないことを記録
        hasInputNodes.insert(endNodeId);
    }

	// ルートノードを特定する
    for (const auto& pair : nodeMap)
    {
        int nodeId = pair.first;
        // 他のどのノードからも繋がれていない（入力がない）ノードを始点とする
        if (hasInputNodes.find(nodeId) == hasInputNodes.end())
        {
            tree.rootAttack = pair.second;
            break; // 見つかったら終了
        }
    }

    return tree;
}