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
		n["name"] = node.name;
		n["is_collapsed"] = node.isCollapsed;

		// アクションノードの場合、アクション名とパラメータも保存
		if (node.type == EditorNodeType::Action)
		{
			// アクション名を保存
			n["action_type"] = static_cast<int32_t>(node.actionType);

			if (node.actionType == ActionType::ComboAttack)
			{
				n["combo_data"]["attackTime"] = node.comboAttackInitData.attackTime;
				n["combo_data"]["moveSpeed"] = node.comboAttackInitData.moveSpeed;
				n["combo_data"]["moveStartTime"] = node.comboAttackInitData.moveStartTime;
				n["combo_data"]["moveEndTime"] = node.comboAttackInitData.moveEndTime;
				n["combo_data"]["grabWeaponStartTime"] = node.comboAttackInitData.grabWeaponStartTime;
				n["combo_data"]["grabWeaponEndTime"] = node.comboAttackInitData.grabWeaponEndTime;
				n["combo_data"]["isGrabWeapon"] = node.comboAttackInitData.isGrabWeapon;
				n["combo_data"]["isThrowWeapon"] = node.comboAttackInitData.isThrowWeapon;
				n["combo_data"]["throwWeaponTime"] = node.comboAttackInitData.throwWeaponTime;
				n["combo_data"]["throwWeaponPower"] = node.comboAttackInitData.throwWeaponPower;
				n["combo_data"]["throwDirection"] = { node.comboAttackInitData.throwDirection.x, node.comboAttackInitData.throwDirection.y, node.comboAttackInitData.throwDirection.z };

				// ヒットグループの配列データを構築
				json hitGroupsJson = json::array();
				for (const auto& group : node.comboAttackInitData.groups)
				{
					json g;
					g["groupId"] = group.groupId;
					g["damageReaction"] = static_cast<int>(group.damageReaction);
					g["startTime"] = group.startTime;
					g["endTime"] = group.endTime;
					g["knockback"] = group.knockback;
					g["knockbackDirection"] = { group.knockbackDirection.x, group.knockbackDirection.y, group.knockbackDirection.z };
					g["damage"] = group.damage;
					hitGroupsJson.push_back(g);
				}
				n["combo_data"]["groups"] = hitGroupsJson;

				// ヒットボックスの配列データを構築
				json hitboxesJson = json::array();
				for (const auto& hitbox : node.comboAttackInitData.hitboxes)
				{
					json h;
					h["hitJoint"] = static_cast<int>(hitbox.jointType);
					h["radius"] = hitbox.radius;
					h["groupId"] = hitbox.groupId;
					hitboxesJson.push_back(h);
				}
				n["combo_data"]["hitDefinitions"] = hitboxesJson;


				n["motionType"] = static_cast<int>(node.motionType);
				n["motionName"] = node.motionName;
			} 
			else if (node.actionType == ActionType::GrabAttack)
			{
				n["grab_data"]["attackTime"] = node.grabAttackInitData.attackTime;
				n["grab_data"]["grabTime"] = node.grabAttackInitData.grabTime;
				n["grab_data"]["hitboxStartTime"] = node.grabAttackInitData.hitboxStartTime;
				n["grab_data"]["hitboxEndTime"] = node.grabAttackInitData.hitboxEndTime;
				n["grab_data"]["moveSpeed"] = node.grabAttackInitData.moveSpeed;
				n["grab_data"]["moveStartTime"] = node.grabAttackInitData.moveStartTime;
				n["grab_data"]["moveEndTime"] = node.grabAttackInitData.moveEndTime;
				n["grab_data"]["grabWeaponStartTime"] = node.grabAttackInitData.grabWeaponStartTime;
				n["grab_data"]["grabWeaponEndTime"] = node.grabAttackInitData.grabWeaponEndTime;
				n["grab_data"]["isGrabWeapon"] = node.grabAttackInitData.isGrabWeapon;
				n["grab_data"]["jointType"] = static_cast<int>(node.grabAttackInitData.jointType);
				n["grab_data"]["isThrowWeapon"] = node.grabAttackInitData.isThrowWeapon;
				n["grab_data"]["throwWeaponTime"] = node.grabAttackInitData.throwWeaponTime;
				n["grab_data"]["throwWeaponPower"] = node.grabAttackInitData.throwWeaponPower;
				n["grab_data"]["throwDirection"] = { node.grabAttackInitData.throwDirection.x, node.grabAttackInitData.throwDirection.y, node.grabAttackInitData.throwDirection.z };

				n["motionType"] = static_cast<int>(node.motionType);
				n["motionName"] = node.motionName;
			}
			else if (node.actionType == ActionType::GrabStrikeAttack)
			{
				n["grab_strike_data"]["attackTime"] = node.grabStrikeAttackInitData.attackTime;
				n["grab_strike_data"]["moveSpeed"] = node.grabStrikeAttackInitData.moveSpeed;
				n["grab_strike_data"]["moveStartTime"] = node.grabStrikeAttackInitData.moveStartTime;
				n["grab_strike_data"]["moveEndTime"] = node.grabStrikeAttackInitData.moveEndTime;
				n["grab_strike_data"]["knockback"] = node.grabStrikeAttackInitData.knockback;
				n["grab_strike_data"]["knockbackDirection"] = { node.grabStrikeAttackInitData.knockbackDirection.x, node.grabStrikeAttackInitData.knockbackDirection.y, node.grabStrikeAttackInitData.knockbackDirection.z };
				n["grab_strike_data"]["isRelease"] = node.grabStrikeAttackInitData.isRelease;
				n["grab_strike_data"]["damageReaction"] = static_cast<int>(node.grabStrikeAttackInitData.damageReaction);

				n["grab_strike_data"]["targetMotionType"] = static_cast<int>(node.targetMotionType);
				n["grab_strike_data"]["targetMotionName"] = node.targetMotionName;

				 // ヒットの配列データを構築
				json hitsJson = json::array();
				for (const auto& def : node.grabStrikeAttackInitData.hits)
				{
					json h;
					h["hitJoint"] = static_cast<int>(def.hitJoint);
					h["hitTime"] = def.hitTime;
					h["damage"] = def.damage;
					hitsJson.push_back(h);
				}

				// グラブストライク攻撃のデータにヒットの配列を追加
				n["grab_strike_data"]["hits"] = hitsJson;

				n["motionType"] = static_cast<int>(node.motionType);
				n["motionName"] = node.motionName;
			}
			else if (node.actionType == ActionType::Avoid)
			{
				n["avoid_data"]["Duration"] = node.avoidInitData.time;
				n["avoid_data"]["Distance"] = node.avoidInitData.distance;
				n["avoid_data"]["LocalDirection"] = { node.avoidInitData.localDirection.x, node.avoidInitData.localDirection.y };
			}
			else if (node.actionType == ActionType::NavMeshMove)
			{
				n["nav_mesh_move_data"]["moveSpeed"] = node.navMeshMoveInitData.moveSpeed;
				n["nav_mesh_move_data"]["stopDistance"] = node.navMeshMoveInitData.stopDistance;
				n["nav_mesh_move_data"]["isDash"] = node.navMeshMoveInitData.isDash;
			}
			else if (node.actionType == ActionType::NavMeshLeaderMove)
			{
				n["nav_mesh_leader_move_data"]["moveSpeed"] = node.navMeshLeaderMoveInitData.moveSpeed;
				n["nav_mesh_leader_move_data"]["stopDistance"] = node.navMeshLeaderMoveInitData.stopDistance;
				n["nav_mesh_leader_move_data"]["isDash"] = node.navMeshLeaderMoveInitData.isDash;
			}
			else if (node.actionType == ActionType::Telegraph)
			{
				n["telegraph_data"]["telegraphTime"] = node.telegraphInitData.time;

				n["motionType"] = static_cast<int>(node.motionType);
				n["motionName"] = node.motionName;
			}
			else if (node.actionType == ActionType::Defense)
			{
				n["defense_data"]["defenseTime"] = node.defenseInitData.defenseTime;
				n["defense_data"]["parryType"] = static_cast<int>(node.defenseInitData.parryType);

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
		else if (node.type == EditorNodeType::UtilitySelector)
		{
			for (auto& [childId, utilityType] : node.childUtilityMap)
			{
				n["child_utility_map"][std::to_string(childId)] = static_cast<int>(utilityType);
			}
		}
		else if (node.type == EditorNodeType::WeightedRandomSelector)
		{
			for (auto& [childId, weight] : node.childWeightMap)
			{
				n["child_weight_map"][std::to_string(childId)] = weight;
			}
		}
		else if (node.type == EditorNodeType::SubTree)
		{
			n["sub_tree_file_name"] = node.subTreeFileName;
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
		l["start_node"] = link.startNodeId;
		l["end_node"] = link.endNodeId;
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
/// @param outNodes 
/// @param outLinks 
void BehaviorTreeSetting::LoadTree(const std::string& fileName, std::vector<EditorNode>& outNodes, std::vector<EditorLink>& outLinks)
{
	// ファイルパスの構築
	std::string filePath = directory_ + folderName_ + "/" + fileName + ".json";
	std::ifstream ifs(filePath);
	if (!ifs.is_open()) return;

	// JSONの読み込み
	json root;
	ifs >> root;

	// 読み込む前に出力用のコンテナをクリア
	outNodes.clear();
	outLinks.clear();

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
			std::string nodeName = n.value("name", "");
			strncpy_s(node.name, nodeName.c_str(), sizeof(node.name) - 1);
			node.isCollapsed = n.value("is_collapsed", false);
			node.needSetPos = true; // 読み込んだノードは位置をImNodesに反映する必要があるのでフラグを立てる

			// nodeの復元部分に追加
			if (node.type == EditorNodeType::Action)
			{
				// アクション名を読み込む
				node.actionType = static_cast<ActionType>(n.value("action_type", 0));

				if (node.actionType == ActionType::ComboAttack && n.contains("combo_data"))
				{
					node.comboAttackInitData.attackTime = n["combo_data"].value("attackTime", 0.0f);
					node.comboAttackInitData.moveSpeed = n["combo_data"].value("moveSpeed", 0.0f);
					node.comboAttackInitData.moveStartTime = n["combo_data"].value("moveStartTime", 0.0f);
					node.comboAttackInitData.moveEndTime = n["combo_data"].value("moveEndTime", 0.0f);
					node.comboAttackInitData.grabWeaponStartTime = n["combo_data"].value("grabWeaponStartTime", 0.0f);
					node.comboAttackInitData.grabWeaponEndTime = n["combo_data"].value("grabWeaponEndTime", 0.0f);
					node.comboAttackInitData.isGrabWeapon = n["combo_data"].value("isGrabWeapon", false);
					node.comboAttackInitData.isThrowWeapon = n["combo_data"].value("isThrowWeapon", false);
					node.comboAttackInitData.throwWeaponTime = n["combo_data"].value("throwWeaponTime", 0.0f);
					node.comboAttackInitData.throwWeaponPower = n["combo_data"].value("throwWeaponPower", 3.0f);
					node.comboAttackInitData.throwDirection.x = n["combo_data"].value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[0];
					node.comboAttackInitData.throwDirection.y = n["combo_data"].value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[1];
					node.comboAttackInitData.throwDirection.z = n["combo_data"].value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[2];
					node.comboAttackInitData.cancelStartTime = 0.0f;
					node.comboAttackInitData.cancelEndTime = 0.0f;
					node.comboAttackInitData.hAttackMotion = MotionManager::GetInstance()->GetMotion(n["motionType"], n["motionName"]);

					
					node.comboAttackInitData.groups.clear();
					if (n["combo_data"].contains("groups") && n["combo_data"]["groups"].is_array())
					{
						for (const auto& g : n["combo_data"]["groups"])
						{
							HitGroupDefinition groupDef;
							groupDef.groupId = g.value("groupId", 0);
							groupDef.damageReaction = static_cast<DamageReaction>(g.value("damageReaction", 0));
							groupDef.startTime = g.value("startTime", 0.0f);
							groupDef.endTime = g.value("endTime", 0.0f);
							groupDef.knockback = g.value("knockback", 0.0f);
							groupDef.damage = g.value("damage", 1);
							if (g.contains("knockbackDirection") && g["knockbackDirection"].is_array() && g["knockbackDirection"].size() == 3)
							{
								groupDef.knockbackDirection.x = g.value("knockbackDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[0];
								groupDef.knockbackDirection.y = g.value("knockbackDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[1];
								groupDef.knockbackDirection.z = g.value("knockbackDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[2];
							}
							else
							{
								groupDef.knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);
							}
							node.comboAttackInitData.groups.push_back(groupDef);
						}
					}

					// 配列の読み込み
					node.comboAttackInitData.hitboxes.clear();
					if (n["combo_data"].contains("hitDefinitions") && n["combo_data"]["hitDefinitions"].is_array())
					{
						for (const auto& h : n["combo_data"]["hitDefinitions"])
						{
							HitboxDefinition hitboxDef;
							hitboxDef.jointType = static_cast<JointType>(h.value("hitJoint", 0));
							hitboxDef.radius = h.value("radius", 0.25f);
							hitboxDef.groupId = h.value("groupId", 0);
							node.comboAttackInitData.hitboxes.push_back(hitboxDef);
						}
					}

					node.motionType = static_cast<MotionType>(n.value("motionType", 0));
					node.motionName = n.value("motionName", "");
				} 
				else if (node.actionType == ActionType::GrabAttack && n.contains("grab_data"))
				{
					node.grabAttackInitData.attackTime = n["grab_data"].value("attackTime", 0.0f);
					node.grabAttackInitData.grabTime = n["grab_data"].value("grabTime", 0.0f);
					node.grabAttackInitData.hitboxStartTime = n["grab_data"].value("hitboxStartTime", 0.0f);
					node.grabAttackInitData.hitboxEndTime = n["grab_data"].value("hitboxEndTime", 0.0f);
					node.grabAttackInitData.moveSpeed = n["grab_data"].value("moveSpeed", 0.0f);
					node.grabAttackInitData.moveStartTime = n["grab_data"].value("moveStartTime", 0.0f);
					node.grabAttackInitData.moveEndTime = n["grab_data"].value("moveEndTime", 0.0f);
					node.grabAttackInitData.grabWeaponStartTime = n["grab_data"].value("grabWeaponStartTime", 0.0f);
					node.grabAttackInitData.grabWeaponEndTime = n["grab_data"].value("grabWeaponEndTime", 0.0f);
					node.grabAttackInitData.isGrabWeapon = n["grab_data"].value("isGrabWeapon", false);
					node.grabAttackInitData.isThrowWeapon = n["grab_data"].value("isThrowWeapon", false);
					node.grabAttackInitData.throwWeaponTime = n["grab_data"].value("throwWeaponTime", 0.0f);
					node.grabAttackInitData.throwWeaponPower = n["grab_data"].value("throwWeaponPower", 3.0f);
					node.grabAttackInitData.throwDirection.x = n["grab_data"].value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[0];
					node.grabAttackInitData.throwDirection.y = n["grab_data"].value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[1];
					node.grabAttackInitData.throwDirection.z = n["grab_data"].value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[2];
					node.grabAttackInitData.jointType = static_cast<JointType>(n["grab_data"].value("jointType", 0));
					node.grabAttackInitData.hAttackMotion = MotionManager::GetInstance()->GetMotion(n["motionType"], n["motionName"]);

					node.motionType = static_cast<MotionType>(n.value("motionType", 0));
					node.motionName = n.value("motionName", "");
				}
				else if (node.actionType == ActionType::GrabStrikeAttack && n.contains("grab_strike_data"))
				{
					node.grabStrikeAttackInitData.attackTime = n["grab_strike_data"].value("attackTime", 0.0f);
					node.grabStrikeAttackInitData.moveSpeed = n["grab_strike_data"].value("moveSpeed", 0.0f);
					node.grabStrikeAttackInitData.moveStartTime = n["grab_strike_data"].value("moveStartTime", 0.0f);
					node.grabStrikeAttackInitData.moveEndTime = n["grab_strike_data"].value("moveEndTime", 0.0f);
					node.grabStrikeAttackInitData.knockback = n["grab_strike_data"].value("knockback", 0.0f);
					node.grabStrikeAttackInitData.hAttackAnimation = MotionManager::GetInstance()->GetMotion(n["motionType"], n["motionName"]);
					node.grabStrikeAttackInitData.isRelease = n["grab_strike_data"].value("isRelease", false);
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
							def.hitJoint = static_cast<JointType>(h.value("hitJoint", 0));
							def.hitTime = h.value("hitTime", 0.0f);
							def.damage = h.value("damage", 10);

							// 読み込んだヒットをノードのリストに追加
							node.grabStrikeAttackInitData.hits.push_back(def);
						}
					}

					node.motionType = static_cast<MotionType>(n.value("motionType", 0));
					node.motionName = n.value("motionName", "");
				}
				else if (node.actionType == ActionType::Avoid)
				{
					if (n.contains("avoid_data") && n["avoid_data"].is_object())
					{
						// avoid_dataの内容をavoidInitDataにコピーする
						const auto& avoidData = n["avoid_data"];

						node.avoidInitData.time = avoidData.value("Duration", 0.0f);
						node.avoidInitData.distance = avoidData.value("Distance", 0.0f);

						if (avoidData.contains("LocalDirection") && avoidData["LocalDirection"].is_array() && avoidData["LocalDirection"].size() == 2)
						{
							node.avoidInitData.localDirection.x = avoidData["LocalDirection"][0].get<float>();
							node.avoidInitData.localDirection.y = avoidData["LocalDirection"][1].get<float>();
						}
					}
				}
				else if (node.actionType == ActionType::NavMeshLeaderMove)
				{
					if (n.contains("nav_mesh_leader_move_data") && n["nav_mesh_leader_move_data"].is_object())
					{
						const auto& moveData = n["nav_mesh_leader_move_data"];
						node.navMeshLeaderMoveInitData.moveSpeed = moveData.value("moveSpeed", 0.0f);
						node.navMeshLeaderMoveInitData.stopDistance = moveData.value("stopDistance", 0.0f);
						node.navMeshLeaderMoveInitData.isDash = moveData.value("isDash", false);
					}
				}
				else if (node.actionType == ActionType::NavMeshMove)
				{
					if (n.contains("nav_mesh_move_data") && n["nav_mesh_move_data"].is_object())
					{
						const auto& moveData = n["nav_mesh_move_data"];
						node.navMeshMoveInitData.moveSpeed = moveData.value("moveSpeed", 0.0f);
						node.navMeshMoveInitData.stopDistance = moveData.value("stopDistance", 0.0f);
						node.navMeshMoveInitData.isDash = moveData.value("isDash", false);
					}
				}
				else if (node.actionType == ActionType::Telegraph)
				{
					if (n.contains("telegraph_data") && n["telegraph_data"].is_object())
					{
						const auto& telegraphData = n["telegraph_data"];
						node.telegraphInitData.time = telegraphData.value("telegraphTime", 0.0f);
						node.telegraphInitData.hAnimation = MotionManager::GetInstance()->GetMotion(static_cast<MotionType>(n.value("motionType", 0)), n.value("motionName", ""));

						node.motionType = static_cast<MotionType>(n.value("motionType", 0));
						node.motionName = n.value("motionName", "");
					}
				}
				else if (node.actionType == ActionType::Defense)
				{
					if (n.contains("defense_data") && n["defense_data"].is_object())
					{
						const auto& defenseData = n["defense_data"];
						node.defenseInitData.defenseTime = defenseData.value("defenseTime", 0.0f);
						node.defenseInitData.parryType = static_cast<ParryType>(defenseData.value("parryType", 0));
						node.motionType = static_cast<MotionType>(n.value("motionType", 0));
						node.motionName = n.value("motionName", "");
					}
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
			else if (node.type == EditorNodeType::UtilitySelector)
			{
				// ユーティリティセレクタの場合は子ノードとそのユーティリティのマッピングを読み込む
				if (n.contains("child_utility_map") && n["child_utility_map"].is_object())
				{
					for (const auto& [key, value] : n["child_utility_map"].items())
					{
						int childId = std::stoi(key);
						UtilityType utilityType = static_cast<UtilityType>(value.get<int>());
						node.childUtilityMap[childId] = utilityType;
					}
				}
			}
			else if (node.type == EditorNodeType::WeightedRandomSelector)
			{
				// WeightedRandomSelectorの場合は子ノードとその重みのマッピングを読み込む
				if (n.contains("child_weight_map") && n["child_weight_map"].is_object())
				{
					for (const auto& [key, value] : n["child_weight_map"].items())
					{
						int childId = std::stoi(key);
						float weight = value.get<float>();
						node.childWeightMap[childId] = weight;
					}
				}
			}
			else if (node.type == EditorNodeType::SubTree)
			{
				// サブツリーノードの場合はサブツリーファイル名を読み込む
				std::string subFileName = n.value("sub_tree_file_name", "");
				strncpy_s(node.subTreeFileName, subFileName.c_str(), sizeof(node.subTreeFileName) - 1);
			}

			outNodes.push_back(node);
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
			link.startNodeId = l.value("start_node", -1); // デフォルト値を-1に設定
			link.endNodeId = l.value("end_node", -1); // デフォルト値を-1に設定
			outLinks.push_back(link);
		}
	}
}