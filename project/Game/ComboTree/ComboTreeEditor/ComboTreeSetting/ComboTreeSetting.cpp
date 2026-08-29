#include "ComboTreeSetting.h"

/// @brief ツリー構造をファイルに保存する
/// @param fileName 
/// @param nodes 
/// @param links 
void ComboTreeSetting::SaveTree(const std::string& fileName, const std::vector<ComboEditorNode>& nodes, const std::vector<ComboEditorLink>& links)
{
	json rootJson;

	// ノードの保存
	rootJson["nodes"] = json::array();
	for (const auto& node : nodes)
	{
		json nodeJson;
		nodeJson["id"] = node.id;
		nodeJson["name"] = node.name.c_str();
		nodeJson["inputPinId"] = node.inputPinId;
		nodeJson["outputInputXPinId"] = node.outputInputXPinId;
		nodeJson["outputInputYPinId"] = node.outputInputYPinId;
		nodeJson["outputInputBPinId"] = node.outputInputBPinId;
		nodeJson["nodeType"] = static_cast<int>(node.nodeType);

		// エディタ上の最新のノード座標を取得して保存
		ImVec2 pos = ImNodes::GetNodeGridSpacePos(node.id);
		nodeJson["pos"] = { pos.x, pos.y };

		nodeJson["animationName"] = node.motionName.c_str();
		nodeJson["targetAnimationName"] = node.targetMotionName.c_str();

		// パラメータ
		if (node.nodeType == ComboNodeType::Combo)
		{
			json comboParams = nodeJson["comboParams"];
			comboParams["attackTime"] = node.comboAttackInitData.attackTime;
			comboParams["moveSpeed"] = node.comboAttackInitData.moveSpeed;
			comboParams["moveStartTime"] = node.comboAttackInitData.moveStartTime;
			comboParams["moveEndTime"] = node.comboAttackInitData.moveEndTime;
			comboParams["cancelStartTime"] = node.comboAttackInitData.cancelStartTime;
			comboParams["cancelEndTime"] = node.comboAttackInitData.cancelEndTime;
			comboParams["chargeCompleteTime"] = node.comboAttackInitData.chargeCompleteTime;
			comboParams["chargeFinishAttackTime"] = node.comboAttackInitData.chargeFinishAttackTime;
			comboParams["chargeTime"] = node.comboAttackInitData.chargeTime;
			comboParams["isChargeAttack"] = node.comboAttackInitData.isChargeAttack;
			comboParams["isGrabWeapon"] = node.comboAttackInitData.isGrabWeapon;
			comboParams["grabWeaponStartTime"] = node.comboAttackInitData.grabWeaponStartTime;
			comboParams["grabWeaponEndTime"] = node.comboAttackInitData.grabWeaponEndTime;
			comboParams["isThrowWeapon"] = node.comboAttackInitData.isThrowWeapon;
			comboParams["throwWeaponTime"] = node.comboAttackInitData.throwWeaponTime;
			comboParams["throwWeaponPower"] = node.comboAttackInitData.throwWeaponPower;
			comboParams["throwDirection"] = { node.comboAttackInitData.throwDirection.x, node.comboAttackInitData.throwDirection.y, node.comboAttackInitData.throwDirection.z };

			// ヒット判定のグループの配列データを構築
			json groupsJson = json::array();
			for (const auto& group : node.comboAttackInitData.groups)
			{
				json groupJson;
				groupJson["groupId"] = group.groupId;
				groupJson["damageReaction"] = static_cast<int>(group.damageReaction);
				groupJson["startTime"] = group.startTime;
				groupJson["endTime"] = group.endTime;
				groupJson["knockback"] = group.knockback;
				groupJson["knockbackDirection"] = { group.knockbackDirection.x, group.knockbackDirection.y, group.knockbackDirection.z };
				groupJson["damage"] = group.damage;
				groupsJson.push_back(groupJson);
			}
			comboParams["groups"] = groupsJson;

			// ヒットボックスの配列データを構築
			json hitboxesJson = json::array();
			for (const auto& hitbox : node.comboAttackInitData.hitboxes)
			{
				json hitboxJson;
				hitboxJson["groupId"] = hitbox.groupId;
				hitboxJson["jointType"] = static_cast<int>(hitbox.jointType);
				hitboxJson["radius"] = hitbox.radius;
				hitboxesJson.push_back(hitboxJson);
			}
			comboParams["hitboxes"] = hitboxesJson;

			// JSONに追加
			nodeJson["comboParams"] = comboParams;
		}
		else if (node.nodeType == ComboNodeType::Grab)
		{
			json grabParams = nodeJson["grabParams"];
			grabParams["attackTime"] = node.grabAttackInitData.attackTime;
			grabParams["moveSpeed"] = node.grabAttackInitData.moveSpeed;
			grabParams["moveStartTime"] = node.grabAttackInitData.moveStartTime;
			grabParams["moveEndTime"] = node.grabAttackInitData.moveEndTime;
			grabParams["grabTime"] = node.grabAttackInitData.grabTime;
			grabParams["hitboxStartTime"] = node.grabAttackInitData.hitboxStartTime;
			grabParams["hitboxEndTime"] = node.grabAttackInitData.hitboxEndTime;
			grabParams["isGrabWeapon"] = node.grabAttackInitData.isGrabWeapon;
			grabParams["grabWeaponStartTime"] = node.grabAttackInitData.grabWeaponStartTime;
			grabParams["grabWeaponEndTime"] = node.grabAttackInitData.grabWeaponEndTime;
			grabParams["jointType"] = static_cast<int>(node.grabAttackInitData.jointType);
			grabParams["isThrowWeapon"] = node.grabAttackInitData.isThrowWeapon;
			grabParams["throwWeaponTime"] = node.grabAttackInitData.throwWeaponTime;
			grabParams["throwWeaponPower"] = node.grabAttackInitData.throwWeaponPower;
			grabParams["throwDirection"] = { node.grabAttackInitData.throwDirection.x, node.grabAttackInitData.throwDirection.y, node.grabAttackInitData.throwDirection.z };

			nodeJson["grabParams"] = grabParams;
		}
		else if (node.nodeType == ComboNodeType::GrabStrike)
		{
			json grabStrikeParams = nodeJson["grabStrikeParams"];
			grabStrikeParams["attackTime"] = node.grabStrikeAttackInitData.attackTime;
			grabStrikeParams["moveSpeed"] = node.grabStrikeAttackInitData.moveSpeed;
			grabStrikeParams["moveStartTime"] = node.grabStrikeAttackInitData.moveStartTime;
			grabStrikeParams["moveEndTime"] = node.grabStrikeAttackInitData.moveEndTime;
			grabStrikeParams["knockback"] = node.grabStrikeAttackInitData.knockback;
			grabStrikeParams["knockbackDirection"] = {
				node.grabStrikeAttackInitData.knockbackDirection.x,
				node.grabStrikeAttackInitData.knockbackDirection.y,
				node.grabStrikeAttackInitData.knockbackDirection.z
			};
			grabStrikeParams["isRelease"] = node.grabStrikeAttackInitData.isRelease;
			grabStrikeParams["damageReaction"] = static_cast<int>(node.grabStrikeAttackInitData.damageReaction);

			grabStrikeParams["chargeTime"] = node.grabStrikeAttackInitData.chargeTime;
			grabStrikeParams["chargeCompleteTime"] = node.grabStrikeAttackInitData.chargeCompleteTime;
			grabStrikeParams["chargeFinishAttackTime"] = node.grabStrikeAttackInitData.chargeFinishAttackTime;
			grabStrikeParams["isChargeAttack"] = node.grabStrikeAttackInitData.isChargeAttack;

			grabStrikeParams["hits"] = json::array();
			for (const auto& hit : node.grabStrikeAttackInitData.hits)
			{
				json hitJson;
				hitJson["damage"] = hit.damage;
				hitJson["hitTime"] = hit.hitTime;
				hitJson["hitJoint"] = static_cast<int>(hit.hitJoint);
				grabStrikeParams["hits"].push_back(hitJson);
			}

			nodeJson["grabStrikeParams"] = grabStrikeParams;
		}

		rootJson["nodes"].push_back(nodeJson);
	}

	// リンクの保存
	rootJson["links"] = json::array();
	for (const auto& link : links)
	{
		json linkJson;
		linkJson["id"] = link.id;
		linkJson["startPinId"] = link.startPinId;
		linkJson["endPinId"] = link.endPinId;
		rootJson["links"].push_back(linkJson);
	}

	// ファイルに書き込み
	std::string filePath = directory_ + folderName_ + "/" + fileName + ".json";
	std::ofstream file(filePath);
	if (file.is_open())
	{
		file << rootJson.dump(4); // インデント4マスで見やすく出力
	}
}

/// @brief ファイルからツリー構造を読み込む
/// @param fileName 
/// @param outNodes 
/// @param outLinks 
void ComboTreeSetting::LoadTree(const std::string& fileName, std::vector<ComboEditorNode>& outNodes, std::vector<ComboEditorLink>& outLinks)
{
	std::string filePath = directory_ + folderName_ + "/" + fileName + ".json";
	std::ifstream file(filePath);
	if (!file.is_open()) return;

	json rootJson;
	file >> rootJson;

	// エディタのデータをクリア
	outNodes.clear();
	outLinks.clear();

	// ノードの復元
	for (const auto& nodeJson : rootJson["nodes"])
	{
		ComboEditorNode node;
		node.id = nodeJson.value("id", 0);
		node.name = nodeJson.value("name", "");
		node.inputPinId = nodeJson.value("inputPinId", -1);
		node.outputInputXPinId = nodeJson.value("outputInputXPinId", -1);
		node.outputInputYPinId = nodeJson.value("outputInputYPinId", -1);
		node.outputInputBPinId = nodeJson.value("outputInputBPinId", -1);
		node.nodeType = static_cast<ComboNodeType>(nodeJson.value("nodeType", 0));
		node.pos.x = nodeJson.value("pos", std::vector<float>{0.0f, 0.0f})[0];
		node.pos.y = nodeJson.value("pos", std::vector<float>{0.0f, 0.0f})[1];

		node.motionName = nodeJson["animationName"].get<std::string>();
		node.targetMotionName = nodeJson.value("targetAnimationName", "");

		if (node.nodeType == ComboNodeType::Combo && nodeJson.contains("comboParams"))
		{
			const auto& comboParams = nodeJson["comboParams"];
			node.comboAttackInitData.attackTime = comboParams.value("attackTime", 0.0f);
			node.comboAttackInitData.moveSpeed = comboParams.value("moveSpeed", 0.0f);
			node.comboAttackInitData.moveStartTime = comboParams.value("moveStartTime", 0.0f);
			node.comboAttackInitData.moveEndTime = comboParams.value("moveEndTime", 0.0f);
			node.comboAttackInitData.cancelStartTime = comboParams.value("cancelStartTime", 0.0f);
			node.comboAttackInitData.cancelEndTime = comboParams.value("cancelEndTime", 0.0f);
			node.comboAttackInitData.isGrabWeapon = comboParams.value("isGrabWeapon", false);
			node.comboAttackInitData.chargeCompleteTime = comboParams.value("chargeCompleteTime", 0.0f);
			node.comboAttackInitData.chargeTime = comboParams.value("chargeTime", 0.0f);
			node.comboAttackInitData.chargeFinishAttackTime = comboParams.value("chargeFinishAttackTime", 0.0f);
			node.comboAttackInitData.isChargeAttack = comboParams.value("isChargeAttack", false);
			node.comboAttackInitData.grabWeaponStartTime = comboParams.value("grabWeaponStartTime", 0.0f);
			node.comboAttackInitData.grabWeaponEndTime = comboParams.value("grabWeaponEndTime", 0.0f);
			node.comboAttackInitData.isThrowWeapon = comboParams.value("isThrowWeapon", false);
			node.comboAttackInitData.throwWeaponTime = comboParams.value("throwWeaponTime", 0.0f);
			node.comboAttackInitData.throwWeaponPower = comboParams.value("throwWeaponPower", 3.0f);
			node.comboAttackInitData.throwDirection = Vector3(
				comboParams.value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[0],
				comboParams.value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[1],
				comboParams.value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[2]
			);

			// ヒット判定のグループの復元
			node.comboAttackInitData.groups.clear();
			if (comboParams.contains("groups") && comboParams["groups"].is_array())
			{
				for (const auto& groupJson : comboParams["groups"])
				{
					HitGroupDefinition group;
					group.groupId = groupJson.value("groupId", 0);
					group.damageReaction = static_cast<DamageReaction>(groupJson.value("damageReaction", 0));
					group.startTime = groupJson.value("startTime", 0.0f);
					group.endTime = groupJson.value("endTime", 0.0f);
					group.knockback = groupJson.value("knockback", 0.0f);
					if (groupJson.contains("knockbackDirection") && groupJson["knockbackDirection"].is_array() && groupJson["knockbackDirection"].size() == 3)
					{
						group.knockbackDirection.x = groupJson["knockbackDirection"][0];
						group.knockbackDirection.y = groupJson["knockbackDirection"][1];
						group.knockbackDirection.z = groupJson["knockbackDirection"][2];
					}
					else
					{
						group.knockbackDirection = Vector3(0.0f, 0.0f, 1.0f);
					}
					group.damage = groupJson.value("damage", 10);
					node.comboAttackInitData.groups.push_back(group);
				}
			}

			// ヒットボックスの復元
			node.comboAttackInitData.hitboxes.clear();
			if (comboParams.contains("hitboxes") && comboParams["hitboxes"].is_array())
			{
				for (const auto& hitboxJson : comboParams["hitboxes"])
				{
					HitboxDefinition hitbox;
					hitbox.groupId = hitboxJson.value("groupId", 0);
					hitbox.jointType = static_cast<JointType>(hitboxJson.value("jointType", 0));
					hitbox.radius = hitboxJson.value("radius", 0.25f);
					node.comboAttackInitData.hitboxes.push_back(hitbox);
				}
			}
		}
		else if (node.nodeType == ComboNodeType::Grab && nodeJson.contains("grabParams"))
		{
			const auto& grabParams = nodeJson["grabParams"];
			node.grabAttackInitData.attackTime = grabParams.value("attackTime", 0.0f);
			node.grabAttackInitData.moveSpeed = grabParams.value("moveSpeed", 0.0f);
			node.grabAttackInitData.moveStartTime = grabParams.value("moveStartTime", 0.0f);
			node.grabAttackInitData.moveEndTime = grabParams.value("moveEndTime", 0.0f);
			node.grabAttackInitData.grabTime = grabParams.value("grabTime", 0.0f);
			node.grabAttackInitData.hitboxStartTime = grabParams.value("hitboxStartTime", 0.0f);
			node.grabAttackInitData.hitboxEndTime = grabParams.value("hitboxEndTime", 0.0f);
			node.grabAttackInitData.isGrabWeapon = grabParams.value("isGrabWeapon", false);
			node.grabAttackInitData.grabWeaponStartTime = grabParams.value("grabWeaponStartTime", 0.0f);
			node.grabAttackInitData.grabWeaponEndTime = grabParams.value("grabWeaponEndTime", 0.0f);
			node.grabAttackInitData.isThrowWeapon = grabParams.value("isThrowWeapon", false);
			node.grabAttackInitData.throwWeaponTime = grabParams.value("throwWeaponTime", 0.0f);
			node.grabAttackInitData.throwWeaponPower = grabParams.value("throwWeaponPower", 3.0f);
			node.grabAttackInitData.throwDirection = Vector3(
				grabParams.value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[0],
				grabParams.value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[1],
				grabParams.value("throwDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[2]
			);
			node.grabAttackInitData.jointType = static_cast<JointType>(grabParams.value("jointType", 0));
		}
		else if (node.nodeType == ComboNodeType::GrabStrike && nodeJson.contains("grabStrikeParams"))
		{
			const auto& grabStrikeParams = nodeJson["grabStrikeParams"];
			node.grabStrikeAttackInitData.attackTime = grabStrikeParams.value("attackTime", 0.0f);
			node.grabStrikeAttackInitData.moveSpeed = grabStrikeParams.value("moveSpeed", 0.0f);
			node.grabStrikeAttackInitData.moveStartTime = grabStrikeParams.value("moveStartTime", 0.0f);
			node.grabStrikeAttackInitData.moveEndTime = grabStrikeParams.value("moveEndTime", 0.0f);
			node.grabStrikeAttackInitData.isRelease = grabStrikeParams.value("isRelease", false);
			node.grabStrikeAttackInitData.knockback = grabStrikeParams.value("knockback", 0.0f);
			node.grabStrikeAttackInitData.knockbackDirection = Vector3(
				grabStrikeParams.value("knockbackDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[0],
				grabStrikeParams.value("knockbackDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[1],
				grabStrikeParams.value("knockbackDirection", std::vector<float>{0.0f, 0.0f, 1.0f})[2]
			);
			node.grabStrikeAttackInitData.damageReaction = static_cast<DamageReaction>(grabStrikeParams.value("damageReaction", 0));

			node.grabStrikeAttackInitData.chargeTime = grabStrikeParams.value("chargeTime", 0.0f);
			node.grabStrikeAttackInitData.chargeCompleteTime = grabStrikeParams.value("chargeCompleteTime", 0.0f);
			node.grabStrikeAttackInitData.chargeFinishAttackTime = grabStrikeParams.value("chargeFinishAttackTime", 0.0f);
			node.grabStrikeAttackInitData.isChargeAttack = grabStrikeParams.value("isChargeAttack", false);

			// 当たり判定配列の復元
			if (grabStrikeParams.contains("hits"))
			{
				for (const auto& defJson : grabStrikeParams["hits"])
				{
					HitDefinition def;
					def.damage = defJson.value("damage", 0);
					def.hitTime = defJson.value("hitTime", 0.0f);
					def.hitJoint = static_cast<JointType>(defJson.value("hitJoint", 0));
					node.grabStrikeAttackInitData.hits.push_back(def);
				}
			}
		}

		outNodes.push_back(node);

		// ImNodesに座標を即座に反映させる（重要）
		ImNodes::SetNodeGridSpacePos(node.id, ImVec2(node.pos.x, node.pos.y));
	}

	// リンクの復元
	for (const auto& linkJson : rootJson["links"])
	{
		ComboEditorLink link;
		link.id = linkJson["id"];
		link.startPinId = linkJson["startPinId"];
		link.endPinId = linkJson["endPinId"];
		outLinks.push_back(link);
	}
}