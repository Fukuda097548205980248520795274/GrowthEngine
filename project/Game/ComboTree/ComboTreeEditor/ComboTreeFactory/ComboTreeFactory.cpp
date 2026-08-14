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
	std::string directoryPath = "./Assets/Parameter/ComboTree/" + jsonFilePath + ".json";

	std::unique_ptr<ComboTree> tree = std::make_unique<ComboTree>();
    std::ifstream file(directoryPath);
    if (!file.is_open()) return nullptr; // 読み込み失敗

    json rootJson;
    file >> rootJson;

    // 一時的な検索用マップ
    std::unordered_map<int, Attack*> nodeMap; // ノードID -> インスタンスポインタ
    std::unordered_map<int, int> pinToNodeMap; // ピンID -> そのピンを持つノードID
    std::unordered_map<int, AttackInputType> pinToTypeMap; // 出力ピンID -> 弱か強か

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
                initData.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, animName);
            }

            initData.attackTime = paramsJson.value("attackTime", 0.0f);
            initData.moveSpeed = paramsJson.value("moveSpeed", 0.0f);
            initData.moveStartTime = paramsJson.value("moveStartTime", 0.0f);
            initData.moveEndTime = paramsJson.value("moveEndTime", 0.0f);
            initData.cancelStartTime = paramsJson.value("cancelStartTime", 0.0f);
            initData.cancelEndTime = paramsJson.value("cancelEndTime", 0.0f);
			initData.isGrabWeapon = paramsJson.value("isGrabWeapon", false);
			initData.grabWeaponStartTime = paramsJson.value("grabWeaponStartTime", 0.0f);
			initData.grabWeaponEndTime = paramsJson.value("grabWeaponEndTime", 0.0f);
			initData.isThrowWeapon = paramsJson.value("isThrowWeapon", false);
			initData.throwWeaponTime = paramsJson.value("throwWeaponTime", 0.0f);
			initData.throwWeaponPower = paramsJson.value("throwWeaponPower", 3.0f);
			initData.throwDirection = Vector3(
				paramsJson.value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[0],
				paramsJson.value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[1],
				paramsJson.value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[2]
			);

            initData.groups.clear();
			if (paramsJson.contains("groups") && paramsJson["groups"].is_array())
			{
				for (const auto& groupJson : paramsJson["groups"])
				{
					HitGroupDefinition groupDef;
					groupDef.groupId = groupJson.value("groupId", 0);
					groupDef.damage = groupJson.value("damage", 10);
					groupDef.damageReaction = static_cast<DamageReaction>(groupJson.value("damageReaction", 0));
					groupDef.knockback = groupJson.value("knockback", 0.0f);
					groupDef.knockbackDirection = Vector3(
						groupJson.value("knockbackDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[0],
						groupJson.value("knockbackDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[1],
						groupJson.value("knockbackDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[2]
					);
					groupDef.startTime = groupJson.value("startTime", 0.0f);
					groupDef.endTime = groupJson.value("endTime", 0.0f);
					initData.groups.push_back(groupDef);
				}
			}

			initData.hitboxes.clear();
            if (paramsJson.contains("hitboxes") && paramsJson["hitboxes"].is_array())
            {
				for (const auto& hitboxJson : paramsJson["hitboxes"])
				{
					HitboxDefinition hitboxDef;
					hitboxDef.groupId = hitboxJson.value("groupId", 0);
					hitboxDef.jointType = static_cast<JointType>(hitboxJson.value("jointType", 0));
					hitboxDef.radius = hitboxJson.value("radius", 0.25f);
					initData.hitboxes.push_back(hitboxDef);
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
                initData.hAttackMotion = MotionManager::GetInstance()->GetMotion(MotionType::Attack, animName);
            }

			initData.attackTime = paramsJson.value("attackTime", 0.0f);
			initData.moveSpeed = paramsJson.value("moveSpeed", 0.0f);
			initData.moveStartTime = paramsJson.value("moveStartTime", 0.0f);
			initData.moveEndTime = paramsJson.value("moveEndTime", 0.0f);
			initData.grabTime = paramsJson.value("grabTime", 0.0f);
			initData.isGrabWeapon = paramsJson.value("isGrabWeapon", false);
			initData.grabWeaponStartTime = paramsJson.value("grabWeaponStartTime", 0.0f);
			initData.grabWeaponEndTime = paramsJson.value("grabWeaponEndTime", 0.0f);
			initData.isThrowWeapon = paramsJson.value("isThrowWeapon", false);
			initData.throwWeaponTime = paramsJson.value("throwWeaponTime", 0.0f);
			initData.throwWeaponPower = paramsJson.value("throwWeaponPower", 3.0f);
			initData.throwDirection = Vector3(
				paramsJson.value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[0],
				paramsJson.value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[1],
				paramsJson.value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[2]
			);

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

			// 攻撃側のアニメーション
			if (!animName.empty())
			{
				initData.hAttackAnimation = MotionManager::GetInstance()->GetMotion(MotionType::Attack, animName);
			}

			// 相手側のアニメーション
			if (!targetAnimName.empty())
			{
				initData.hTargetAnimation = MotionManager::GetInstance()->GetMotion(MotionType::Stagger, targetAnimName);
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
			initData.damageReaction = static_cast<DamageReaction>(paramsJson.value("damageReaction", 0));

            if (paramsJson.contains("hits"))
            {
                for (const auto& defJson : paramsJson["hits"])
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