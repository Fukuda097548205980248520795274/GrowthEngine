#include "BehaviorTreeSetting.h"

/// @brief ツリー構造をファイルに保存する
/// @param fileName 
/// @param nodes 
/// @param links 
void BehaviorTreeSetting::SaveTree(const std::string& fileName, const std::vector<EditorNode>& nodes, const std::vector<EditorLink>& links)
{

    json root;

	// ノードの保存
    for (const auto& node : nodes) 
    {
        json n;
        n["id"] = node.id;
        n["type"] = node.type;
        n["pos"] = { node.pos.x, node.pos.y };
        n["input_pin"] = node.inputPinId;
        n["output_pin"] = node.outputPinId;

        if (node.type == EditorNodeType::Condition)
        {
            n["condition_type"] = static_cast<int>(node.conditionType);
        }

        // アクションノードの場合、アクション名とパラメータも保存
        if (node.type == EditorNodeType::Action)
        {
			// アクション名を保存
			n["action_name"] = node.actionName;

            if (node.actionName == "ComboAttack")
            {
                n["combo_data"]["attackTime"] = node.comboAttackInitData.attackTime;
                n["combo_data"]["moveSpeed"] = node.comboAttackInitData.moveSpeed;
                n["combo_data"]["damage"] = node.comboAttackInitData.damage;
                n["combo_data"]["hitboxStartTime"] = node.comboAttackInitData.hitboxStartTime;
				n["combo_data"]["hitboxEndTime"] = node.comboAttackInitData.hitboxEndTime;
				n["combo_data"]["jointType"] = static_cast<int>(node.comboAttackInitData.jointType);
				n["combo_data"]["moveStartTime"] = node.comboAttackInitData.moveStartTime;
				n["combo_data"]["moveEndTime"] = node.comboAttackInitData.moveEndTime;
				n["combo_data"]["cancelStartTime"] = node.comboAttackInitData.cancelStartTime;
				n["combo_data"]["cancelEndTime"] = node.comboAttackInitData.cancelEndTime;
				n["combo_data"]["damageReaction"] = static_cast<int>(node.comboAttackInitData.damageReaction);
				n["combo_data"]["knockback"] = node.comboAttackInitData.knockback;
            } 
            else if (node.actionName == "GrabAttack")
            {
                n["grab_data"]["attackTime"] = node.grabAttackInitData.attackTime;
                n["grab_data"]["grabTime"] = node.grabAttackInitData.grabTime;
                n["grab_data"]["hitboxStartTime"] = node.grabAttackInitData.hitboxStartTime;
				n["grab_data"]["hitboxEndTime"] = node.grabAttackInitData.hitboxEndTime;
                n["grab_data"]["moveSpeed"] = node.grabAttackInitData.moveSpeed;
				n["grab_data"]["moveStartTime"] = node.grabAttackInitData.moveStartTime;
				n["grab_data"]["moveEndTime"] = node.grabAttackInitData.moveEndTime;
				n["grab_data"]["jointType"] = static_cast<int>(node.grabAttackInitData.jointType);
            }

			if (node.actionName != "None")
            {
				n["motionType"] = static_cast<int>(node.motionType);
				n["motionName"] = node.motionName;
            }
        }

        root["nodes"].push_back(n);
    }

	// リンクの保存
    for (const auto& link : links)
    {
        json l;
        l["id"] = link.id;
        l["start"] = link.startPinId;
        l["end"] = link.endPinId;
        root["links"].push_back(l);
    }

	// ファイルパスの構築
    std::string filePath = directory_ + folderName_ + "/" + fileName + ".json";
    std::ofstream ofs(filePath);
    if (ofs.is_open()) 
    {
		// JSONを整形して保存
        ofs << root.dump(4);
    }
}

/// @brief ファイルからツリー構造を読み込む
/// @param fileName 
/// @param out_nodes 
/// @param out_links 
void BehaviorTreeSetting::LoadTree(const std::string& fileName, std::vector<EditorNode>& out_nodes, std::vector<EditorLink>& out_links)
{
	// ファイルパスの構築
    std::string filePath = directory_ + folderName_ + "/" + fileName + ".json";
    std::ifstream ifs(filePath);
    if (!ifs.is_open()) return;

	// JSONの読み込み
    json root;
    ifs >> root;

	// 読み込む前に出力用のコンテナをクリア
    out_nodes.clear();
    out_links.clear();

	// ノードの読み込み
    if (root.contains("nodes")) 
    {
        for (auto& n : root["nodes"]) 
        {
            EditorNode node;
            node.id = n["id"];
            node.type = n["type"];
            node.pos.x = n["pos"][0];
            node.pos.y = n["pos"][1];
            node.inputPinId = n["input_pin"];
            node.outputPinId = n["output_pin"];

            // nodeの復元部分に追加
            if (node.type == EditorNodeType::Action)
            {
				// アクション名を読み込む
				node.actionName = n.value("action_name", "None");

                if (node.actionName == "ComboAttack" && n.contains("combo_data"))
                {
                    node.comboAttackInitData.attackTime = n["combo_data"].value("attackTime", 0.0f);
                    node.comboAttackInitData.moveSpeed = n["combo_data"].value("moveSpeed", 0.0f);
                    node.comboAttackInitData.damage = n["combo_data"].value("damage", 10);
					node.comboAttackInitData.hitboxStartTime = n["combo_data"].value("hitboxStartTime", 0.0f);
					node.comboAttackInitData.hitboxEndTime = n["combo_data"].value("hitboxEndTime", 0.0f);
					node.comboAttackInitData.jointType = static_cast<JointType>(n["combo_data"].value("jointType", 0));
					node.comboAttackInitData.moveStartTime = n["combo_data"].value("moveStartTime", 0.0f);
					node.comboAttackInitData.moveEndTime = n["combo_data"].value("moveEndTime", 0.0f);
					node.comboAttackInitData.cancelStartTime = n["combo_data"].value("cancelStartTime", 0.0f);
					node.comboAttackInitData.cancelEndTime = n["combo_data"].value("cancelEndTime", 0.0f);
					node.comboAttackInitData.damageReaction = static_cast<DamageReaction>(n["combo_data"].value("damageReaction", 0));
					node.comboAttackInitData.knockback = n["combo_data"].value("knockback", 0.0f);
                    node.comboAttackInitData.hAttackMotion = MotionManager::GetInstance()->GetMotion(n["motionType"], n["motionName"]);
                } 
                else if (node.actionName == "GrabAttack" && n.contains("grab_data"))
                {
                    node.grabAttackInitData.attackTime = n["grab_data"].value("attackTime", 0.0f);
                    node.grabAttackInitData.grabTime = n["grab_data"].value("grabTime", 0.0f);
					node.grabAttackInitData.hitboxStartTime = n["grab_data"].value("hitboxStartTime", 0.0f);
					node.grabAttackInitData.hitboxEndTime = n["grab_data"].value("hitboxEndTime", 0.0f);
					node.grabAttackInitData.moveSpeed = n["grab_data"].value("moveSpeed", 0.0f);
					node.grabAttackInitData.moveStartTime = n["grab_data"].value("moveStartTime", 0.0f);
					node.grabAttackInitData.moveEndTime = n["grab_data"].value("moveEndTime", 0.0f);
					node.grabAttackInitData.jointType = static_cast<JointType>(n["grab_data"].value("jointType", 0));
					node.grabAttackInitData.hAttackMotion = MotionManager::GetInstance()->GetMotion(n["motionType"], n["motionName"]);
                }

                if (node.actionName != "None")
                {
                    node.motionType = static_cast<MotionType>(n.value("motionType", 0));
                    node.motionName = n.value("motionName", "");
                }
            }

			// 条件ノードの場合は条件の種類も読み込む
            if (node.type == EditorNodeType::Condition)
            {
				node.conditionType = static_cast<ConditionType>(n.value("condition_type", 0));
            }

            out_nodes.push_back(node);
        }
    }

	// リンクの読み込み
    if (root.contains("links")) 
    {
        for (auto& l : root["links"])
        {
            EditorLink link;
            link.id = l["id"];
            link.startPinId = l["start"];
            link.endPinId = l["end"];
            out_links.push_back(link);
        }
    }
}