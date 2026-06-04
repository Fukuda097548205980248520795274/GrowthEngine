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
		n["is_collapsed"] = node.isCollapsed;

        // アクションノードの場合、アクション名とパラメータも保存
        if (node.type == EditorNodeType::Action)
        {
			// アクション名を保存
			n["action_name"] = node.actionName;

            if (node.actionName == "ComboAttack")
            {
                n["combo_data"]["attackTime"] = node.comboAttackInitData.attackTime;
                n["combo_data"]["moveSpeed"] = node.comboAttackInitData.moveSpeed;
				n["combo_data"]["moveStartTime"] = node.comboAttackInitData.moveStartTime;
				n["combo_data"]["moveEndTime"] = node.comboAttackInitData.moveEndTime;

                // 当たり判定の配列データを構築
                json hitboxesJson = json::array();
                for (const auto& def : node.comboAttackInitData.hitDefinitions)
                {
                    json h;
                    h["jointType"] = static_cast<int>(def.jointType);
                    h["startTime"] = def.startTime;
                    h["endTime"] = def.endTime;
                    h["radius"] = def.radius;
                    h["damage"] = def.damage;
                    h["damageReaction"] = static_cast<int>(def.damageReaction);
                    h["knockback"] = def.knockback;
                    h["knockbackDirection"] = { def.knockbackDirection.x, def.knockbackDirection.y, def.knockbackDirection.z };
                    hitboxesJson.push_back(h);
                }

				// コンボ攻撃のデータに当たり判定の配列を追加
                n["combo_data"]["hitDefinitions"] = hitboxesJson;
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
            else if (node.actionName == "GrabStrikeAttack")
            {
                n["grab_strike_data"]["attackTime"] = node.grabStrikeAttackInitData.attackTime;
                n["grab_strike_data"]["moveSpeed"] = node.grabStrikeAttackInitData.moveSpeed;
				n["grab_strike_data"]["moveStartTime"] = node.grabStrikeAttackInitData.moveStartTime;
				n["grab_strike_data"]["moveEndTime"] = node.grabStrikeAttackInitData.moveEndTime;
				n["grab_strike_data"]["knockback"] = node.grabStrikeAttackInitData.knockback;
				n["grab_strike_data"]["knockbackDirection"] = { node.grabStrikeAttackInitData.knockbackDirection.x, node.grabStrikeAttackInitData.knockbackDirection.y, node.grabStrikeAttackInitData.knockbackDirection.z };
				n["grab_strike_data"]["isRelease"] = node.grabStrikeAttackInitData.isRelease;
				n["grab_strike_data"]["releaseTime"] = node.grabStrikeAttackInitData.releaseTime;
                n["grab_strike_data"]["damageReaction"] = static_cast<int>(node.grabStrikeAttackInitData.damageReaction);

				n["grab_strike_data"]["targetMotionType"] = static_cast<int>(node.targetMotionType);
				n["grab_strike_data"]["targetMotionName"] = node.targetMotionName;

                 // ヒットの配列データを構築
				json hitsJson = json::array();
				for (const auto& def : node.grabStrikeAttackInitData.hits)
				{
					json h;
					h["targetHitJoint"] = static_cast<int>(def.targetHitJoint);
					h["hitTime"] = def.hitTime;
					h["damage"] = def.damage;
					hitsJson.push_back(h);
				}

				// グラブストライク攻撃のデータにヒットの配列を追加
				n["grab_strike_data"]["hits"] = hitsJson;
            }
            else if (node.actionName == "Avoid")
            {
				n["avoid_data"]["Duration"] = node.avoidInitData.time;
				n["avoid_data"]["Distance"] = node.avoidInitData.distance;
				n["avoid_data"]["LocalDirection"] = { node.avoidInitData.localDirection.x, node.avoidInitData.localDirection.y };
            }
            else if (node.actionName == "ApproachTargetMove")
            {
				n["approach_target_move_data"]["moveSpeed"] = node.approachTargetMoveInitData.moveSpeed;
				n["approach_target_move_data"]["stopDistance"] = node.approachTargetMoveInitData.stopDistance;
            }
            else if (node.actionName == "NavMeshMove")
            {
				n["nav_mesh_move_data"]["moveSpeed"] = node.navMeshMoveInitData.moveSpeed;
				n["nav_mesh_move_data"]["stopDistance"] = node.navMeshMoveInitData.stopDistance;
            }

			if (node.actionName != "None")
            {
				n["motionType"] = static_cast<int>(node.motionType);
				n["motionName"] = node.motionName;
            }
        }
		else if (node.type == EditorNodeType::Condition)
		{
			// 条件ノードのパラメータを設定
			n["condition_type"] = static_cast<int>(node.conditionType);

			// 条件の種類によって保存するパラメータを変える
			if (node.conditionType == ConditionType::IsTargetInRange || node.conditionType == ConditionType::IsTargetOutOfRange)
			{
				n["condition_param"]["distance_to_target"] = node.conditionParam.distanceToTarget;
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
			node.isCollapsed = n.value("is_collapsed", false);
			node.needSetPos = true; // 読み込んだノードは位置をImNodesに反映する必要があるのでフラグを立てる

            // nodeの復元部分に追加
            if (node.type == EditorNodeType::Action)
            {
				// アクション名を読み込む
				node.actionName = n.value("action_name", "None");

                if (node.actionName == "ComboAttack" && n.contains("combo_data"))
                {
                    node.comboAttackInitData.attackTime = n["combo_data"].value("attackTime", 0.0f);
                    node.comboAttackInitData.moveSpeed = n["combo_data"].value("moveSpeed", 0.0f);
					node.comboAttackInitData.moveStartTime = n["combo_data"].value("moveStartTime", 0.0f);
					node.comboAttackInitData.moveEndTime = n["combo_data"].value("moveEndTime", 0.0f);
					node.comboAttackInitData.cancelStartTime = 0.0f;
					node.comboAttackInitData.cancelEndTime = 0.0f;
                    node.comboAttackInitData.hAttackMotion = MotionManager::GetInstance()->GetMotion(n["motionType"], n["motionName"]);

                    // 配列の読み込み
                    node.comboAttackInitData.hitDefinitions.clear();
                    if (n["combo_data"].contains("hitDefinitions") && n["combo_data"]["hitDefinitions"].is_array())
                    {
                        for (const auto& h : n["combo_data"]["hitDefinitions"])
                        {
                            HitboxDefinition def;
                            def.jointType = static_cast<JointType>(h.value("jointType", 0));
                            def.startTime = h.value("startTime", 0.0f);
                            def.endTime = h.value("endTime", 0.0f);
                            def.radius = h.value("radius", 0.25f);
                            def.damage = h.value("damage", 1);
                            def.damageReaction = static_cast<DamageReaction>(h.value("damageReaction", 0));
                            def.knockback = h.value("knockback", 0.0f);

                            if (h.contains("knockbackDirection") && h["knockbackDirection"].is_array() && h["knockbackDirection"].size() == 3)
                            {
                                def.knockbackDirection.x = h["knockbackDirection"][0];
                                def.knockbackDirection.y = h["knockbackDirection"][1];
                                def.knockbackDirection.z = h["knockbackDirection"][2];
                            }
                            else
                            {
                                def.knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);
                            }

							// 読み込んだ当たり判定をノードのリストに追加
                            node.comboAttackInitData.hitDefinitions.push_back(def);
                        }
                    }
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
                else if (node.actionName == "GrabStrikeAttack" && n.contains("grab_strike_data"))
                {
                    node.grabStrikeAttackInitData.attackTime = n["grab_strike_data"].value("attackTime", 0.0f);
                    node.grabStrikeAttackInitData.moveSpeed = n["grab_strike_data"].value("moveSpeed", 0.0f);
					node.grabStrikeAttackInitData.moveStartTime = n["grab_strike_data"].value("moveStartTime", 0.0f);
					node.grabStrikeAttackInitData.moveEndTime = n["grab_strike_data"].value("moveEndTime", 0.0f);
					node.grabStrikeAttackInitData.knockback = n["grab_strike_data"].value("knockback", 0.0f);
					node.grabStrikeAttackInitData.hAttackAnimation = MotionManager::GetInstance()->GetMotion(n["motionType"], n["motionName"]);
					node.grabStrikeAttackInitData.isRelease = n["grab_strike_data"].value("isRelease", false);
					node.grabStrikeAttackInitData.releaseTime = n["grab_strike_data"].value("releaseTime", 0.0f);
					node.grabStrikeAttackInitData.damageReaction = static_cast<DamageReaction>(n["grab_strike_data"].value("damageReaction", 0));

					// ノックバックの方向は配列で保存されているので、読み込むときは配列からVector3に変換する
                    if (n.contains("grab_strike_data") && n["grab_strike_data"].contains("knockbackDirection") && 
                        n["grab_strike_data"]["knockbackDirection"].is_array() && n["grab_strike_data"]["knockbackDirection"].size() == 3)
                    {
                        node.grabStrikeAttackInitData.knockbackDirection.x = n["grab_strike_data"]["knockbackDirection"][0];
                        node.grabStrikeAttackInitData.knockbackDirection.y = n["grab_strike_data"]["knockbackDirection"][1];
                        node.grabStrikeAttackInitData.knockbackDirection.z = n["grab_strike_data"]["knockbackDirection"][2];
                    }
                    else
                    {
                        node.grabStrikeAttackInitData.knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);
                    }

					node.targetMotionName = n["grab_strike_data"].value("targetMotionName", "");
					node.targetMotionType = static_cast<MotionType>(n["grab_strike_data"].value("targetMotionType", 0));
					node.grabStrikeAttackInitData.hTargetAnimation = MotionManager::GetInstance()->GetMotion(node.targetMotionType, node.targetMotionName);

                    // 配列の読み込み
                    node.grabStrikeAttackInitData.hits.clear();
                    if (n["grab_strike_data"].contains("hits") && n["grab_strike_data"]["hits"].is_array())
                    {
                        for (const auto& h : n["grab_strike_data"]["hits"])
                        {
                            HitDefinition def;
                            def.targetHitJoint = static_cast<JointType>(h.value("targetHitJoint", 0));
                            def.hitTime = h.value("hitTime", 0.0f);
                            def.damage = h.value("damage", 10);

							// 読み込んだヒットをノードのリストに追加
							node.grabStrikeAttackInitData.hits.push_back(def);
                        }
                    }
                }
                else if (node.actionName == "Avoid")
                {
                    if (n.contains("avoid_data") && n["avoid_data"].is_object())
                    {
                        // 参照を作っておくと、記述がスッキリし、無駄なアクセスも減ります
                        const auto& avoid_data = n["avoid_data"];

                        node.avoidInitData.time = avoid_data.value("Duration", 0.0f);
                        node.avoidInitData.distance = avoid_data.value("Distance", 0.0f);

                        if (avoid_data.contains("LocalDirection") &&
                            avoid_data["LocalDirection"].is_array() &&
                            avoid_data["LocalDirection"].size() == 2)
                        {
                            // 2. get<float>() を使って明示的に型を変換して代入する
                            // ※ x, y が double 型の場合は get<double>() に変更してください
                            node.avoidInitData.localDirection.x = avoid_data["LocalDirection"][0].get<float>();
                            node.avoidInitData.localDirection.y = avoid_data["LocalDirection"][1].get<float>();
                        }
                    }
                }
                else if (node.actionName == "ApproachTargetMove")
                {
                    if (n.contains("approach_target_move_data") && n["approach_target_move_data"].is_object())
                    {
						const auto& move_data = n["approach_target_move_data"];
						node.approachTargetMoveInitData.moveSpeed = move_data.value("moveSpeed", 0.0f);
						node.approachTargetMoveInitData.stopDistance = move_data.value("stopDistance", 0.0f);
                    }
                }
				else if (node.actionName == "NavMeshMove")
				{
                    if (n.contains("nav_mesh_move_data") && n["nav_mesh_move_data"].is_object())
                    {
                        const auto& move_data = n["nav_mesh_move_data"];
                        node.navMeshMoveInitData.moveSpeed = move_data.value("moveSpeed", 0.0f);
                        node.navMeshMoveInitData.stopDistance = move_data.value("stopDistance", 0.0f);
                    }
				}

                if (node.actionName != "None")
                {
                    node.motionType = static_cast<MotionType>(n.value("motionType", 0));
                    node.motionName = n.value("motionName", "");
                }
            }
            else if (node.type == EditorNodeType::Condition)
            {
                // 条件ノードの場合は条件の種類も読み込む

				node.conditionType = static_cast<ConditionType>(n.value("condition_type", 0));

				// 条件の種類によって読み込むパラメータを変える
                if (node.conditionType == ConditionType::IsTargetInRange || node.conditionType == ConditionType::IsTargetOutOfRange)
                {
					node.conditionParam.distanceToTarget = n["condition_param"].value("distance_to_target", 0.0f);
                }
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