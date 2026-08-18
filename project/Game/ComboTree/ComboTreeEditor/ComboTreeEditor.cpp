#include "ComboTreeEditor.h"
#include "GrowthEngine.h"
#include <unordered_set>

using json = nlohmann::json;

/// @brief コンストラクタ
ComboTreeEditor::ComboTreeEditor()
{
	// プロジェクトマネージャー、クリップボード、履歴管理クラスのインスタンスを作成
	projectManager_ = std::make_unique<ComboTreeProjectManager>();
	clipboard_ = std::make_unique<ComboTreeEditorClipboard>();
	history_ = std::make_unique<ComboTreeEditorHistory>();
}

/// @brief ノードを追加する
void ComboTreeEditor::AddComboAttackNode()
{
	// 変更があったことを通知
	HandleChange();

	// 新しいノードを作成
	ComboEditorNode node;
	node.id = GetNextId();
	node.inputPinId = GetNextId();
	node.outputInputXPinId = GetNextId();
	node.outputInputYPinId = GetNextId();
	node.outputInputBPinId = GetNextId();
	node.nodeType = ComboNodeType::Combo;

	// 現在のウィンドウ（ノードエディタ）の位置とサイズを取得して中央の座標を計算
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 centerPos = ImVec2(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);

	// ImNodesのAPIを使って、いま見えている画面の中央座標にノードを配置する
	ImNodes::SetNodeScreenSpacePos(node.id, centerPos);

	// 無効な座標を設定しておく
	node.pos = ImVec2(-99999.0f, -99999.0f);

	// ノードを配列に追加
	nodes_.push_back(node);
}

/// @brief ノードを追加する
void ComboTreeEditor::AddGrabAttackNode()
{
	// 変更があったことを通知
	HandleChange();

	// 新しいノードを作成
	ComboEditorNode node;
	node.id = GetNextId();
	node.inputPinId = GetNextId();
	node.outputInputXPinId = GetNextId();
	node.outputInputYPinId = GetNextId();
	node.outputInputBPinId = GetNextId();
	node.nodeType = ComboNodeType::Grab;

	// 現在のウィンドウ（ノードエディタ）の位置とサイズを取得して中央の座標を計算
	ImVec2 windowPos = ImGui::GetWindowPos();
	ImVec2 windowSize = ImGui::GetWindowSize();
	ImVec2 centerPos = ImVec2(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);

	// ImNodesのAPIを使って、いま見えている画面の中央座標にノードを配置する
	ImNodes::SetNodeScreenSpacePos(node.id, centerPos);

	// 無効な座標を設定しておく
	node.pos = ImVec2(-99999.0f, -99999.0f);

	// ノードを配列に追加
	nodes_.push_back(node);
}

/// @brief ノードを追加する
void ComboTreeEditor::AddGrabStrikeAttackNode()
{
	// 変更があったことを通知
	HandleChange();

	// 新しいノードを作成
	ComboEditorNode node;
	node.id = GetNextId();
	node.inputPinId = GetNextId();
	node.outputInputXPinId = GetNextId();
	node.outputInputYPinId = GetNextId();
	node.outputInputBPinId = GetNextId();
	node.nodeType = ComboNodeType::GrabStrike;

	// 現在のウィンドウ（ノードエディタ）の位置とサイズを取得して中央の座標を計算
	ImVec2 centerPos = ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x * 0.5f, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y * 0.5f);

	// ImNodesのAPIを使って、いま見えている画面の中央座標にノードを配置する
	ImNodes::SetNodeScreenSpacePos(node.id, centerPos);

	// 無効な座標を設定しておく
	node.pos = ImVec2(-99999.0f, -99999.0f);

	nodes_.push_back(node);
}

/// @brief ファイルにコンボツリーを保存する
/// @param filePath 
void ComboTreeEditor::SaveToFile(const std::string& filePath)
{
	json rootJson;

	// カウンタの保存
	rootJson["currentId"] = currentId_;

	// ノードの保存
	rootJson["nodes"] = json::array();
	for (const auto& node : nodes_)
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
			comboParams["attackTime"] = node.comboAttackInitData.attackTime;
			comboParams["moveSpeed"] = node.comboAttackInitData.moveSpeed;
			comboParams["moveStartTime"] = node.comboAttackInitData.moveStartTime;
			comboParams["moveEndTime"] = node.comboAttackInitData.moveEndTime;
			comboParams["cancelStartTime"] = node.comboAttackInitData.cancelStartTime;
			comboParams["cancelEndTime"] = node.comboAttackInitData.cancelEndTime;
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
	for (const auto& link : links_)
	{
		json linkJson;
		linkJson["id"] = link.id;
		linkJson["startPinId"] = link.startPinId;
		linkJson["endPinId"] = link.endPinId;
		rootJson["links"].push_back(linkJson);
	}

	// ファイルに書き込み
	std::ofstream file(filePath);
	if (file.is_open())
	{
		file << rootJson.dump(4); // インデント4マスで見やすく出力
	}
}

/// @brief ファイルからコンボツリーを読み込む
/// @param filePath 
void ComboTreeEditor::LoadFromFile(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (!file.is_open()) return;

	json rootJson;
	file >> rootJson;

	// エディタのデータをクリア
	nodes_.clear();
	links_.clear();

	// カウンタの復元
	currentId_ = rootJson["currentId"];

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

		nodes_.push_back(node);

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
		links_.push_back(link);
	}
}

/// @brief UI描画処理
void ComboTreeEditor::DrawUI()
{
	if (!ImGui::GetIO().WantTextInput && !ImGui::IsAnyItemActive())
	{
		ImGuiIO& io = ImGui::GetIO();

		// Ctrl + Sで保存
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_S))
		{
			if (!currentFileName_.empty())
			{
				SaveToFile("./Assets/Parameter/ComboTree/" + currentFileName_ + ".json");
			}
		}

		// Ctrl + Cでコピー
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C))
		{
			clipboard_->HandleCopy(nodes_, links_);
		}

		// Ctrl + Vで貼り付け
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V))
		{
			clipboard_->HandlePaste(*this);
		}

		// Ctrl + Zで元に戻す
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))
		{
			history_->Undo(*this);
		}

		// Ctrl + Yでやり直す
		if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y))
		{
			history_->Redo(*this);
		}

		// Deleteキー Backspaceキー で選択されたノードを削除
		if (ImNodes::NumSelectedNodes() > 0 && (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace)))
		{
			DeleteSelectedNodes();

		}

		// Ctrl + 右クリック でリンクを削除
		DeleteLink();

		// Homeキー または Ctrl + F でルートノードにカメラを移動
		if (ImGui::IsKeyPressed(ImGuiKey_Home) || (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_F)))
		{
			int rootId = FindRootNodeId();
			if (rootId != -1)
			{
				ImNodes::EditorContextMoveToNode(rootId);
			}
		}
	}

	DrawProjectPanel();
	DrawNodeEditor();
	DrawPropertyPanel();
}

/// @brief プロジェクトパネルを描画する
void ComboTreeEditor::DrawProjectPanel()
{
	ImGui::Begin("Combo Tree Browser");


	// 新規作成ボタン
	if (ImGui::Button("新規作成", ImVec2(0, 30)))
	{
		// ポップアップを開くフラグを立てる
		ImGui::OpenPopup("Create New File Popup");
	}

	if (ImGui::BeginPopupModal("Create New File Popup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		static char newFileName[128] = "";

		ImGui::Text("Enter new combo tree name:");
		ImGui::InputText("##NewFileName", newFileName, sizeof(newFileName));

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		// 作成ボタン
		if (ImGui::Button("Create", ImVec2(120, 0)))
		{
			if (strlen(newFileName) > 0)
			{
				currentFileName_ = newFileName;
				nodes_.clear();
				links_.clear();
				SaveToFile("./Assets/Parameter/ComboTree/" + currentFileName_ + ".json");

				// バッファをクリアしてポップアップを閉じる
				newFileName[0] = '\0';
				ImGui::CloseCurrentPopup();
			}
		}

		ImGui::SameLine();

		// キャンセルボタン
		if (ImGui::Button("Cancel", ImVec2(120, 0)))
		{
			newFileName[0] = '\0'; // キャンセル時もクリアしておく
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
	

	ImGui::Separator();
	ImGui::Text("File List:");

	// プロジェクトマネージャーからファイル一覧を取得
	std::vector<std::string> fileList = projectManager_->GetFileList();

	// タイルのサイズ設定
	const float kThumbnailSize = 64.0f;
	const float kPadding = 12.0f;
	const float kCellSize = kThumbnailSize + kPadding;

	// パネルの幅に応じて列数を計算
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columnCount = static_cast<int>(panelWidth / kCellSize);
	if (columnCount < 1) columnCount = 1; // 最低でも1列は確保

	if (ImGui::BeginTable("AssetGrid", columnCount, ImGuiTableFlags_None))
	{
		for (const auto& fileName : fileList)
		{
			ImGui::TableNextColumn();
			ImGui::PushID(fileName.c_str());

			bool isSelected = (currentFileName_ == fileName);

			// 選択されている場合のみ、ボタンの色をハイライトする
			if (isSelected)
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.17f, 0.35f, 0.52f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.2f, 0.4f, 0.6f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.17f, 0.35f, 0.52f, 1.0f));
			}

			// 正方形のボタン領域（ホバー・クリックに反応するのはこの四角形のみ）
			if (ImGui::Button(fileName.c_str(), ImVec2(kThumbnailSize, kThumbnailSize)))
			{
				currentFileName_ = fileName;
				LoadFromFile("./Assets/Parameter/ComboTree/" + currentFileName_ + ".json");
			}

			if (isSelected)
			{
				ImGui::PopStyleColor(3);
			}

			// ボタンの下に表示する名前
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
			ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + kThumbnailSize);
			ImGui::TextUnformatted(fileName.c_str());
			ImGui::PopTextWrapPos();

			ImGui::PopID();
		}
		ImGui::EndTable();
	}

	ImGui::End();
}

/// @brief ノードエディタを描画する
void ComboTreeEditor::DrawNodeEditor()
{
	ImGui::Begin("Combo Tree");

	// ファイルが選択されていない場合はノードエディタを描画せずに終了
	if (currentFileName_.empty())
	{
		ImGui::End();
		return;
	}

	// テスト用にノードを追加するボタン
	if (ImGui::Button("Add Combo Node"))
	{
		AddComboAttackNode();
	}
	ImGui::SameLine();
	if (ImGui::Button("Add Grab Node"))
	{
		AddGrabAttackNode();
	}
	ImGui::SameLine();
	if (ImGui::Button("Add Grab Strike Node"))
	{
		AddGrabStrikeAttackNode();
	}


	// ノードの位置変更を検知して履歴に保存する処理
	for (auto& node : nodes_)
	{
		ImVec2 gridPos = ImNodes::GetNodeGridSpacePos(node.id);

		// 初期状態の座標が無効値の場合は、ImNodesの座標をそのまま保存する
		if (node.pos.x == -99999.0f && node.pos.y == -99999.0f)
		{
			node.pos = gridPos;
			continue;
		}

		// ノードがドラッグされている間、座標が変化したら履歴に保存する
		if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
		{
			if (gridPos.x != node.pos.x || gridPos.y != node.pos.y)
			{
				// ドラッグが始まった「最初の1フレームだけ」、動く前の状態を履歴に保存する
				if (!isDraggingNode_)
				{
					HandleChange();
					isDraggingNode_ = true;
				}
				node.pos = gridPos; // 座標を同期
			}
		}
	}

	// マウスの左クリックが離されたら、ドラッグ終了フラグをリセット
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
	{
		isDraggingNode_ = false;
	}


	ImNodes::BeginNodeEditor();

	for (const auto& node : nodes_)
	{
		// ノードの描画開始
		ImNodes::BeginNode(node.id);

		// ノードのタイトルバー
		ImNodes::BeginNodeTitleBar();
		if (!node.name.empty())
		{
			ImGui::Text("%s", node.name.c_str());
		}
		else
		{
			if (node.nodeType == ComboNodeType::Combo)
			{
				ImGui::Text("Combo Attack");
			} 
			else if (node.nodeType == ComboNodeType::Grab)
			{
				ImGui::Text("Grab Attack");
			}
			else if (node.nodeType == ComboNodeType::GrabStrike)
			{
				ImGui::Text("Grab Strike Attack");
			}
		}
		ImNodes::EndNodeTitleBar();

		// 左側：入力ピン
		ImNodes::BeginInputAttribute(node.inputPinId);
		ImGui::Text("In");
		ImNodes::EndInputAttribute();

		ImGui::Spacing();

		// 右側：X入力攻撃派生ピン
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(100, 200, 250, 255));
		ImNodes::BeginOutputAttribute(node.outputInputXPinId);
		ImGui::Indent(40.0f);
		ImGui::Text("X 入力");
		ImNodes::EndOutputAttribute();
		ImNodes::PopColorStyle();

		// 右側：Y入力攻撃派生ピン
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(250, 100, 100, 255));
		ImNodes::BeginOutputAttribute(node.outputInputYPinId);
		ImGui::Indent(40.0f);
		ImGui::Text("Y 入力");
		ImNodes::EndOutputAttribute();
		ImNodes::PopColorStyle();

		// 右側：B入力攻撃派生ピン
		ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(100, 250, 100, 255));
		ImNodes::BeginOutputAttribute(node.outputInputBPinId);
		ImGui::Indent(40.0f);
		ImGui::Text("B 入力");
		ImNodes::EndOutputAttribute();
		ImNodes::PopColorStyle();

		// ノードの描画終了
		ImNodes::EndNode();
	}

	// リンクの描画
	for (const auto& link : links_)
	{
		if (link.linkType == ComboTreeInputType::X) 
		{
			ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(100, 200, 250, 255)); // 水色
		}
		else if (link.linkType == ComboTreeInputType::Y) 
		{
			ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(250, 100, 100, 255)); // 赤色
		}
		else if (link.linkType == ComboTreeInputType::B)
		{
			ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(100, 250, 100, 255)); // 緑色
		}

		ImNodes::Link(link.id, link.startPinId, link.endPinId);

		// リンクの色を元に戻す
		if (link.linkType != ComboTreeInputType::None) 
		{
			ImNodes::PopColorStyle();
		}
	}

	ImNodes::EndNodeEditor();



	// リンクが作成されたときの処理
	int startPinId, endPinId;
	if (ImNodes::IsLinkCreated(&startPinId, &endPinId))
	{
		// ピンIDから、接続元(start)と接続先(end)のノードを探す
		ComboEditorNode* startNode = nullptr;
		ComboEditorNode* endNode = nullptr;
		for (auto& node : nodes_) {
			if (node.outputInputXPinId == startPinId || node.outputInputYPinId == startPinId || node.outputInputBPinId == startPinId) startNode = &node;
			if (node.inputPinId == endPinId) endNode = &node;
		}

		// 接続元と接続先のノードが見つかった場合のみリンクを作成する
		if (startNode && endNode)
		{
			bool canConnect = true;

			// 接続先が「つかみ打撃」の場合、接続元は「つかみ」か「つかみ打撃」でなければならない
			if (endNode->nodeType == ComboNodeType::GrabStrike)
			{
				if (startNode->nodeType != ComboNodeType::Grab && startNode->nodeType != ComboNodeType::GrabStrike)
				{
					canConnect = false;
				}
			}

			// 接続元が「つかみ」の場合、接続先は「つかみ打撃」以外つなげない
			if (startNode->nodeType == ComboNodeType::Grab)
			{
				if (endNode->nodeType != ComboNodeType::GrabStrike)
				{
					canConnect = false;
				}
			}

			// 接続元が「つかみ打撃」の場合、接続先は「つかみ打撃」以外つなげない
			if (startNode->nodeType == ComboNodeType::GrabStrike)
			{
				if (endNode->nodeType != ComboNodeType::GrabStrike)
				{
					canConnect = false;
				}
			}

			// 制限をクリアした場合のみリンクを生成する
			if (canConnect)
			{
				ComboEditorLink link;
				link.id = GetNextId();
				link.startPinId = startPinId;
				link.endPinId = endPinId;
				links_.push_back(link);
				HandleChange(); // 変更履歴へ
			}
		}
	}

	ImGui::End();
}

/// @brief プロパティパネルを描画する
void ComboTreeEditor::DrawPropertyPanel()
{
	// プロパティウィンドウの描画
	ImGui::Begin("Combo Properties");

	// 選択されているノードの数を取得
	int numSelected = ImNodes::NumSelectedNodes();

	if (numSelected == 1)
	{
		// 1つだけ選択されている場合、そのノードのIDを取得
		int selectedNodeId;
		ImNodes::GetSelectedNodes(&selectedNodeId);

		ComboEditorNode* node = GetNodeById(selectedNodeId);
		if (node)
		{

			ImGui::Text("Attack Motion");
			// 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
			std::vector<std::string> motionNames = MotionManager::GetInstance()->GetMotionNames(MotionType::Attack);
			std::string motionName = node->motionName;

			// モーション名のリストが空の場合はエラーメッセージを表示
			if (motionNames.empty())
			{
				ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
			}
			else
			{
				// 現在選択されているモーション名をプレビュー用の文字列として設定
				const char* previewValue = motionName.empty() ? "モーションを選択..." : motionName.c_str();

				// モーション名選択用のコンボボックスを描画
				if (ImGui::BeginCombo("攻撃モーション", previewValue))
				{
					for (const auto& name : motionNames)
					{
						// 現在のモーション名と同じものが選択されている状態にする
						bool isSelected = (motionName == name);
						if (ImGui::Selectable(name.c_str(), isSelected))
						{
							//history_->SaveHistory(nodes_, links_, currentId_);
							//isDirty_ = true;

							node->motionName = name; // 選択されたモーション名をノードに設定
						}
						if (isSelected) ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}

			if (node->nodeType == ComboNodeType::Combo)
			{
				ImGui::Separator();
				ImGui::DragFloat("Attack Time", &node->comboAttackInitData.attackTime, 0.01f, 0.0f, 10.0f);

				ImGui::Spacing();
				ImGui::Text("Movement");
				ImGui::DragFloat("Move Speed", &node->comboAttackInitData.moveSpeed, 0.1f, 0.0f, 100.0f);
				ImGui::DragFloat("Move Start Time", &node->comboAttackInitData.moveStartTime, 0.01f, 0.0f, node->comboAttackInitData.attackTime);
				ImGui::DragFloat("Move End Time", &node->comboAttackInitData.moveEndTime, 0.01f, 0.0f, node->comboAttackInitData.attackTime);

				ImGui::Spacing();
				ImGui::Text("Cancel");
				ImGui::DragFloat("Cancel Start Time", &node->comboAttackInitData.cancelStartTime, 0.01f, 0.0f, node->comboAttackInitData.attackTime);
				ImGui::DragFloat("Cancel End Time", &node->comboAttackInitData.cancelEndTime, 0.01f, 0.0f, node->comboAttackInitData.attackTime);

				ImGui::Spacing();

				if (!node->comboAttackInitData.isGrabWeapon)
				{
					ImGui::Text("Throw Weapon");
					ImGui::Checkbox("Is Throw Weapon", &node->comboAttackInitData.isThrowWeapon);
				}

				if (!node->comboAttackInitData.isThrowWeapon)
				{
					ImGui::Text("Grab Weapon");
					ImGui::Checkbox("Is Grab Weapon", &node->comboAttackInitData.isGrabWeapon);
				}
				

				if (node->comboAttackInitData.isThrowWeapon)
				{
					node->comboAttackInitData.isGrabWeapon = false; // 投げ武器が有効な場合、つかみ武器は無効にする

					ImGui::DragFloat("Throw Weapon Time", &node->comboAttackInitData.throwWeaponTime, 0.01f, 0.0f, node->comboAttackInitData.attackTime);
					ImGui::DragFloat("Throw Weapon Power", &node->comboAttackInitData.throwWeaponPower, 0.01f, 0.0f, 100000.0f);
					ImGui::DragFloat3("Throw Direction", &node->comboAttackInitData.throwDirection.x, 0.05f);

					// 投げ方向を正規化
					node->comboAttackInitData.throwDirection = node->comboAttackInitData.throwDirection.Normalize();
				}

				if (node->comboAttackInitData.isGrabWeapon)
				{
					node->comboAttackInitData.isThrowWeapon = false; // つかみ武器が有効な場合、投げ武器は無効にする

					ImGui::DragFloat("Grab Weapon Start Time", &node->comboAttackInitData.grabWeaponStartTime, 0.01f, 0.0f, node->comboAttackInitData.attackTime);
					ImGui::DragFloat("Grab Weapon End Time", &node->comboAttackInitData.grabWeaponEndTime, 0.01f, 0.0f, node->comboAttackInitData.attackTime);
				}

				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Text("Hitboxes (当たり判定)");

				// ヒットグループ設定
				if (ImGui::TreeNode("ヒットグループ設定（ダメージ・属性）"))
				{
					auto& groups = node->comboAttackInitData.groups;

					if (ImGui::Button("+ グループ追加"))
					{
						HitGroupDefinition newGroup;
						newGroup.groupId = static_cast<int32_t>(groups.size());
						groups.push_back(newGroup);
					}

					for (size_t i = 0; i < groups.size(); ++i)
					{
						ImGui::PushID(static_cast<int>(i));
						std::string label = "グループ ID: " + std::to_string(groups[i].groupId);
						if (ImGui::TreeNode(label.c_str()))
						{
							ImGui::InputInt("グループID", &groups[i].groupId);
							ImGui::DragFloat("開始時間", &groups[i].startTime, 0.01f);
							ImGui::DragFloat("終了時間", &groups[i].endTime, 0.01f);
							ImGui::InputInt("ダメージ", &groups[i].damage);

							// ダメージリアクション
							const char* damageReactionNames[] = { "None", "LightStagger", "HeavyStagger", "Down" };
							int currentReaction = static_cast<int>(groups[i].damageReaction);
							if (ImGui::Combo("ダメージリアクション", &currentReaction, damageReactionNames, IM_ARRAYSIZE(damageReactionNames)))
							{
								history_->SaveHistory(nodes_, links_, currentId_);

								groups[i].damageReaction = static_cast<DamageReaction>(currentReaction);
							}

							ImGui::DragFloat("ノックバック力", &groups[i].knockback, 0.1f);
							ImGui::DragFloat3("ノックバック方向", &groups[i].knockbackDirection.x, 0.05f);

							if (ImGui::Button("削除"))
							{
								history_->SaveHistory(nodes_, links_, currentId_);

								groups.erase(groups.begin() + i);
								ImGui::TreePop();
								ImGui::PopID();
								break;
							}
							ImGui::TreePop();
						}
						ImGui::PopID();
					}
					ImGui::TreePop();
				}

				ImGui::Separator();

				// 当たり判定の配置設定
				if (ImGui::TreeNode("当たり判定の配置（部位・時間）"))
				{
					auto& hitDefs = node->comboAttackInitData.hitboxes;

					if (ImGui::Button("+ 当たり判定追加"))
					{
						hitDefs.push_back(HitboxDefinition());
					}

					for (size_t i = 0; i < hitDefs.size(); ++i)
					{
						ImGui::PushID(static_cast<int>(1000 + i));
						if (ImGui::TreeNode((std::string("判定 ") + std::to_string(i + 1)).c_str()))
						{
							ImGui::InputInt("所属グループID", &hitDefs[i].groupId);

							// ジョイントタイプ
							int currentJoint = static_cast<int>(hitDefs[i].jointType);
							if (ImGui::Combo("ジョイントタイプ", &currentJoint, jointTypeNames, IM_ARRAYSIZE(jointTypeNames)))
							{
								history_->SaveHistory(nodes_, links_, currentId_);

								hitDefs[i].jointType = static_cast<JointType>(currentJoint);
							}

							ImGui::DragFloat("半径", &hitDefs[i].radius, 0.01f);

							if (ImGui::Button("削除"))
							{
								history_->SaveHistory(nodes_, links_, currentId_);

								hitDefs.erase(hitDefs.begin() + i);
								ImGui::TreePop();
								ImGui::PopID();
								break;
							}
							ImGui::TreePop();
						}
						ImGui::PopID();
					}

					ImGui::TreePop();
				}
			}
			else if (node->nodeType == ComboNodeType::Grab)
			{
				ImGui::Separator();
				ImGui::DragFloat("Attack Time", &node->grabAttackInitData.attackTime, 0.01f, 0.0f, 10.0f);

				ImGui::Spacing();
				ImGui::Text("Movement");
				ImGui::DragFloat("Move Speed", &node->grabAttackInitData.moveSpeed, 0.1f, 0.0f, 100.0f);
				ImGui::DragFloat("Move Start Time", &node->grabAttackInitData.moveStartTime, 0.01f, 0.0f, node->grabAttackInitData.attackTime);
				ImGui::DragFloat("Move End Time", &node->grabAttackInitData.moveEndTime, 0.01f, 0.0f, node->grabAttackInitData.attackTime);

				ImGui::Spacing();
				ImGui::Text("Grab Weapon");
				ImGui::Checkbox("Is Grab Weapon", &node->grabAttackInitData.isGrabWeapon);
				if (node->grabAttackInitData.isGrabWeapon)
				{
					ImGui::DragFloat("Grab Weapon Start Time", &node->grabAttackInitData.grabWeaponStartTime, 0.01f, 0.0f, node->grabAttackInitData.attackTime);
					ImGui::DragFloat("Grab Weapon End Time", &node->grabAttackInitData.grabWeaponEndTime, 0.01f, 0.0f, node->grabAttackInitData.attackTime);
				}


				ImGui::Spacing();
				ImGui::Separator();
				ImGui::Text("Hitboxes (当たり判定)");
				ImGui::DragFloat("Hitbox Start Time", &node->grabAttackInitData.hitboxStartTime, 0.01f, 0.0f, node->grabAttackInitData.attackTime);
				ImGui::DragFloat("Hitbox End Time", &node->grabAttackInitData.hitboxEndTime, 0.01f, 0.0f, node->grabAttackInitData.attackTime);

				// ジョイントタイプ
				int currentJoint = static_cast<int>(node->grabAttackInitData.jointType);
				if (ImGui::Combo("Joint", &currentJoint, jointTypeNames, IM_ARRAYSIZE(jointTypeNames)))
				{
					node->grabAttackInitData.jointType = static_cast<JointType>(currentJoint);
				}
			}
			else if (node->nodeType == ComboNodeType::GrabStrike)
			{
				ImGui::Separator();
				ImGui::DragFloat("Attack Time", &node->grabStrikeAttackInitData.attackTime, 0.01f, 0.0f, 10.0f);

				ImGui::Text("Release");
				ImGui::Checkbox("Is Release", &node->grabStrikeAttackInitData.isRelease);
				
				if (node->grabStrikeAttackInitData.isRelease)
				{
					ImGui::Spacing();
					ImGui::Text("Movement");
					ImGui::DragFloat("Move Speed", &node->grabStrikeAttackInitData.moveSpeed, 0.1f, 0.0f, 100.0f);
					ImGui::DragFloat("Move Start Time", &node->grabStrikeAttackInitData.moveStartTime, 0.01f, 0.0f, node->grabStrikeAttackInitData.attackTime);
					ImGui::DragFloat("Move End Time", &node->grabStrikeAttackInitData.moveEndTime, 0.01f, 0.0f, node->grabStrikeAttackInitData.attackTime);

					ImGui::Spacing();
					ImGui::Text("Knockback");
					ImGui::DragFloat("Knockback", &node->grabStrikeAttackInitData.knockback, 0.1f, 0.0f, 100.0f);
					ImGui::DragFloat3("Knockback Dir", &node->grabStrikeAttackInitData.knockbackDirection.x, 0.01f);
				}


				// 相手のやられモーションの選択
				std::vector<std::string> targetMotionNames = MotionManager::GetInstance()->GetMotionNames(MotionType::Stagger);
				std::string targetMotionName = node->targetMotionName;

				// モーション名のリストが空の場合はエラーメッセージを表示
				if (targetMotionNames.empty())
				{
					ImGui::TextColored(ImVec4(1, 0, 0, 1), "モーションがロードされていません");
				} 
				else
				{
					// 現在選択されているモーション名をプレビュー用の文字列として設定
					const char* previewValue = targetMotionName.empty() ? "モーションを選択..." : targetMotionName.c_str();

					// モーション名選択用のコンボボックスを描画
					if (ImGui::BeginCombo("相手のやられモーション", previewValue))
					{
						for (const auto& name : targetMotionNames)
						{
							// 現在のモーション名と同じものが選択されている状態にする
							bool isSelected = (targetMotionName == name);
							if (ImGui::Selectable(name.c_str(), isSelected))
							{
								node->targetMotionName = name; // 選択されたモーション名をノードに設定
							}
							if (isSelected) ImGui::SetItemDefaultFocus();
						}
						ImGui::EndCombo();
					}
				}

				// 列挙型 (DamageReaction) のコンボボックス
				if (node->grabStrikeAttackInitData.isRelease)
				{
					const char* reactionNames[] = { "None", "LightStagger", "HeavyStagger", "Down", "Deflected", "Repelled" };
					int currentReaction = static_cast<int>(node->grabStrikeAttackInitData.damageReaction);
					if (ImGui::Combo("Reaction", &currentReaction, reactionNames, IM_ARRAYSIZE(reactionNames)))
					{
						node->grabStrikeAttackInitData.damageReaction = static_cast<DamageReaction>(currentReaction);
					}
				}


				// 新しい当たり判定を追加するボタン
				if (ImGui::Button("Add Hitbox"))
				{
					node->grabStrikeAttackInitData.hits.push_back(HitDefinition());
				}
				
				// 配列の各要素を描画
				for (int i = 0; i < node->grabStrikeAttackInitData.hits.size(); ++i)
				{
					// ImGuiのID衝突を避けるためにインデックスでPushする
					ImGui::PushID(i);

					// 折りたたみ可能なツリーノードでまとめる
					char hitboxName[32];
					sprintf_s(hitboxName, "Hitbox [%d]", i);
					if (ImGui::TreeNode(hitboxName))
					{
						HitDefinition& def = node->grabStrikeAttackInitData.hits[i];

						ImGui::DragInt("damage", &def.damage, 1, 0, 9999);
						ImGui::DragFloat("hitTime", &def.hitTime, 0.01f, 0.0f, node->grabStrikeAttackInitData.attackTime);

						// ジョイントタイプ
						int currentJoint = static_cast<int>(def.hitJoint);
						if (ImGui::Combo("hitJoint", &currentJoint, jointTypeNames, IM_ARRAYSIZE(jointTypeNames)))
						{
							def.hitJoint = static_cast<JointType>(currentJoint);
						}

						ImGui::Spacing();

						// 削除ボタン
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
						if (ImGui::Button("Delete Hitbox"))
						{
							node->grabStrikeAttackInitData.hits.erase(node->grabStrikeAttackInitData.hits.begin() + i);
							ImGui::PopStyleColor();
							ImGui::TreePop();
							ImGui::PopID();
							--i; // 要素を削除したのでインデックスを戻す
							continue; // ループの次へ
						}
						ImGui::PopStyleColor();

						ImGui::TreePop(); // TreeNodeの終了
					}

					ImGui::PopID(); // PushIDの終了
				}
			}
		}
	}
	else
	{
		ImGui::Text("ノードが選択されていません");
	}

	ImGui::End();
}

/// @brief リンクを削除する
void ComboTreeEditor::DeleteLink()
{
	int hoveredLinkId;

	// マウスカーソルがリンクの上に乗っているかを取得
	if (ImNodes::IsLinkHovered(&hoveredLinkId))
	{
		ImGuiIO& io = ImGui::GetIO();

		// Ctrlキーが押されている ＆ 左クリックが押された瞬間
		if (io.KeyCtrl && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
		{
			// 変更があったことを通知
			HandleChange();

			// 対象のリンクIDを探して配列から削除
			for (auto it = links_.begin(); it != links_.end(); )
			{
				if (it->id == hoveredLinkId)
				{
					it = links_.erase(it); // 削除
					break; // 同じIDのリンクは他にないのでループを抜ける
				}
				else
				{
					++it;
				}
			}
		}
	}
}

/// @brief 選択されているノードを削除する
void ComboTreeEditor::DeleteSelectedNodes()
{
	// 選択されているノードの数を取得
	int numSelectedNodes = ImNodes::NumSelectedNodes();
	if (numSelectedNodes == 0) return;

	// 消す前の状態を履歴に保存
	HandleChange();

	// 選択されているすべてのノードIDを取得
	std::vector<int> selectedNodeIds(numSelectedNodes);
	ImNodes::GetSelectedNodes(selectedNodeIds.data());

	for (int nodeId : selectedNodeIds)
	{
		// 削除対象のノードに繋がっているリンクを先に削除
		ComboEditorNode* node = GetNodeById(nodeId);
		if (node)
		{
			int inPin = node->inputPinId;
			int outInputXPin = node->outputInputXPinId;
			int outInputYPin = node->outputInputYPinId;
			int outInputBPin = node->outputInputBPinId;

			for (auto it = links_.begin(); it != links_.end(); )
			{
				if (it->startPinId == inPin || it->startPinId == outInputXPin || it->startPinId == outInputYPin || it->startPinId == outInputBPin ||
					it->endPinId == inPin || it->endPinId == outInputXPin || it->endPinId == outInputYPin || it->endPinId == outInputBPin)
				{
					it = links_.erase(it); // リンク配列から削除
				}
				else
				{
					++it;
				}
			}
		}

		// ノード自体の削除
		for (auto it = nodes_.begin(); it != nodes_.end(); )
		{
			if (it->id == nodeId)
			{
				it = nodes_.erase(it); // ノード配列から削除
			}
			else
			{
				++it;
			}
		}

		// ImNodes側の選択キャッシュをクリア (ゴミ残りの防止)
		ImNodes::ClearNodeSelection(nodeId);
	}
}

/// @brief 変更があったことを通知する関数
void ComboTreeEditor::HandleChange()
{
	history_->SaveHistory(nodes_, links_, currentId_);
}

/// @brief 指定されたIDのノードを取得する関数
/// @param id 
/// @return 
ComboEditorNode* ComboTreeEditor::GetNodeById(int id)
{
	for (auto& node : nodes_)
	{
		if (node.id == id) return &node;
	}
	return nullptr;
}

/// @brief ルートノードのIDを取得する関数
/// @return 
int ComboTreeEditor::FindRootNodeId() const
{
	if (nodes_.empty()) return -1;

	// すべてのリンクの終了ピン（入力ピン）をハッシュセットにまとめる
	std::unordered_set<int> connectedInputPins;
	for (const auto& link : links_)
	{
		connectedInputPins.insert(link.endPinId);
	}

	// リンクがどこからも繋がっていない入力ピンを持つノードを探す
	for (const auto& node : nodes_)
	{
		if (connectedInputPins.find(node.inputPinId) == connectedInputPins.end())
		{
			return node.id; // これがルートノード
		}
	}

	// 複雑な循環参照などで見つからなかった場合は、先頭のノードを暫定ルートとする
	return nodes_.front().id;
}