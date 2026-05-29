#include "../BehaviorTreeEditor.h"

/// @brief ノードテーブルを描画する
void BehaviorTreeEditor::DrawNodeTable()
{
#ifdef _DEVELOPMENT

    ImGui::Begin("Behavior Tree Editor");

    // 新規ツリーボタン
    if (ImGui::Button("New Tree"))
    {
        ImGui::OpenPopup("NewTreePopup");
    }

    // 新規ツリーポップアップ
    if (ImGui::BeginPopup("NewTreePopup"))
    {
        // 新しいツリーのファイル名入力
        static char newFileName[64] = "";
        ImGui::InputText("File Name", newFileName, 64);

        // ツリー作成ボタン
        if (ImGui::Button("Create"))
        {
            // ファイル名が空でないことを確認
            currentFileName_ = newFileName;
            ClearEditor();
            SaveCurrentTree();

            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::SameLine();


    // 保存されているツリーの一覧を表示
    ImGui::Text("Saved Trees:");
    auto files = projectManager_.GetFileList();

    // 各ファイルに対して選択、コピー、削除のUIを表示
    for (const auto& file : files)
    {
        // ファイル名をIDとしてプッシュ
        ImGui::PushID(file.c_str());

        // 現在のファイルが選択されているかどうかを判定
        bool isSelected = (currentFileName_ == file);

        // ファイル名を選択可能なアイテムとして表示
        if (ImGui::Selectable(file.c_str(), isSelected, 0, ImVec2(ImGui::GetContentRegionAvail().x - 840, 0)))
        {
            LoadTree(file);
        }

        ImGui::SameLine();

        // コピー用のボタンを表示
        if (ImGui::Button("Copy"))
        {
            ImGui::OpenPopup("CopyPopup");
        }

        // コピー用のポップアップ
        if (ImGui::BeginPopup("CopyPopup"))
        {
            // コピー先のファイル名入力
            static char newFileName[64] = "";
            ImGui::Text("Copy to:");
            ImGui::InputText("##newname", newFileName, 64);

            // コピー実行ボタン
            if (ImGui::Button("Execute Copy"))
            {
                // ファイル名が空でないことを確認してコピーを実行
                if (strlen(newFileName) > 0)
                {
                    projectManager_.CopyProjectFile(file, newFileName);
                    ImGui::CloseCurrentPopup();

                    // コピー後のファイル名をリセット
                    memset(newFileName, 0, sizeof(newFileName));
                }
            }
            ImGui::EndPopup();
        }

        ImGui::SameLine();


        // 削除用のボタンを表示（赤色にする）
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
        if (ImGui::Button("Del"))
        {
            ImGui::OpenPopup("DeletePopup");
        }
        ImGui::PopStyleColor();

        // 削除用のポップアップ
        if (ImGui::BeginPopupModal("DeletePopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
        {
            // 削除確認のメッセージを表示
            ImGui::Text("Are you sure you want to delete '%s'?", file.c_str());
            ImGui::Separator();

            // 削除実行ボタン
            if (ImGui::Button("Yes, Delete", ImVec2(120, 0)))
            {
                // ファイルを削除
                projectManager_.DeleteProjectFile(file);

                // 現在編集中のファイルが削除された場合はエディタをクリア
                if (currentFileName_ == file)
                {
                    ClearEditor();
                    currentFileName_ = "";
                }
                ImGui::CloseCurrentPopup();
            }

            // デフォルトで削除実行ボタンにフォーカスを当てる
            ImGui::SetItemDefaultFocus();

            ImGui::SameLine();

            // 削除キャンセルボタン
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }


        ImGui::PopID();
    }

    ImGui::SameLine();

    // 現在のファイル名を表示して保存ボタンを配置
    if (!currentFileName_.empty())
    {
        ImGui::Text("Editing: %s", currentFileName_.c_str());
        if (ImGui::Button("Save (Ctrl+S)"))
        {
            SaveCurrentTree();
        }
    }

    // ファイルが選択されていない場合はノードエディタを描画せずに終了
    if (currentFileName_.empty())
    {
        ImGui::End();
        return;
    }



    // ノード追加ボタン
    if (ImGui::Button("Add Persistent Selector")) AddPersistentSelectorNode();
    ImGui::SameLine();
    if (ImGui::Button("Add Persistent Sequence")) AddPersistentSequenceNode();
    ImGui::SameLine();
    if (ImGui::Button("Add Restarting Selector")) AddRestartingSelectorNode();
    ImGui::SameLine();
    if (ImGui::Button("Add Restarting Sequence")) AddRestartingSequenceNode();
    ImGui::SameLine();
    if (ImGui::Button("Add Condition")) AddConditionNode();
    ImGui::SameLine();
    if (ImGui::Button("Add Action")) AddActionNode();


	// ノードエディタのキャンバスを描画
    DrawNodeEditorCanvas();


    // コピーとペーストの処理
    clipboard_->HandleCopy(nodes_, links_);
    clipboard_->HandlePaste(*this);

    // DeleteキーまたはBackspaceキーが押された場合、選択されているノードを削除する
    if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace))
    {
        DeleteSelectedNodes();
    }

    // Ctrl+ZでUndo、Ctrl+YでRedoを実行する
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))
    {
        history_->Undo(*this);
    }

    // Ctrl+YでRedoを実行
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y))
    {
        history_->Redo(*this);
    }

#endif
}

/// @brief プロパティウィンドウを描画する
void BehaviorTreeEditor::DrawPropertyWindow()
{
#ifdef _DEVELOPMENT

	// プロパティウィンドウの開始
    ImGui::Begin("Node Properties");

    ImNode::SetCurrentEditor(nodeEditorContext_);

    // 選択されているオブジェクト（ノードやリンク）の数を取得
    int numSelected = ImNode::GetSelectedObjectCount();

    // まず、選択されているノードだけを取得したいので配列を用意する
    std::vector<ImNode::NodeId> selectedNodes(numSelected);
    int nodeCount = ImNode::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));

    if (nodeCount == 1)
    {
        // 1つだけ選択されている場合、そのノードのIDを取得
        int selectedNodeId = static_cast<int>(selectedNodes[0].Get());

        // IDからノードを検索
        auto it = std::find_if(nodes_.begin(), nodes_.end(),
            [selectedNodeId](const EditorNode& n) { return n.id == selectedNodeId; });

        if (it != nodes_.end())
        {
            EditorNode& node = *it;

            // ノードの種類などを表示
            ImGui::Text("Node ID: %d", node.id);
            ImGui::Separator();

            // ノードの種類に応じて、パラメータ設定UIをここで描画する
            if (node.type == EditorNodeType::Condition)
            {
                DrawCondtionNodeSettings(node);
            }
            else if (node.type == EditorNodeType::Action)
            {
                DrawActionNodeSettings(node);
            }
            else
            {
                ImGui::Text("No properties to edit for this node type.");
            }
        }
    }
    else if (numSelected > 1)
    {
        // 複数選択時のメッセージ
        ImGui::Text("Multiple nodes selected.");
        ImGui::Text("Please select only one node to edit its properties.");
    }
    else
    {
        // 未選択時のメッセージ
        ImGui::Text("No node selected.");
    }

    ImNode::SetCurrentEditor(nullptr);

    ImGui::End();

#endif
}

/// @brief ノードエディタのキャンバスを描画する
void BehaviorTreeEditor::DrawNodeEditorCanvas()
{
	// ピンIDからノードIDと出力ピンか入力ピンかを取得するラムダ関数
    auto getNodeFromPin = [this](int pinID, bool& outIsOutput) -> int
        {
            for (const auto& node : nodes_)
            {
                // 出力ピンが一致する場合はそのノードIDを返す
                if (node.inputPinId == pinID)
                {
                    outIsOutput = false;
                    return node.id;
                }

                // 入力ピンが一致する場合はそのノードIDを返す
                if (node.outputPinId == pinID)
                {
                    outIsOutput = true;
                    return node.id;
                }
            }

			// どちらも一致しない場合は-1を返す
            return -1;
        };

	// ノードIDをキー、子ノードIDのリストを値とする隣接リストを作成
    std::unordered_map<int, std::vector<int>> adjList;

	// すべてのリンクを処理して隣接リストを構築
	for (const auto& link : links_)
	{
        // 開始ピンIDからノードIDと出力ピンか入力ピンかを取得
		bool startIsOutput = false;
		int startNode = getNodeFromPin(link.startPinId, startIsOutput);

        // 終了ピンIDからノードIDと出力ピンか入力ピンかを取得
		bool endIsOutput = false;
		int endNode = getNodeFromPin(link.endPinId, endIsOutput);

		// どちらも有効なノードIDが取得できた場合のみ隣接リストに追加
        if (startNode != -1 && endNode != -1)
        {
			// 出力ピンから入力ピンへのリンクの場合は、開始ノードを親、終了ノードを子として隣接リストに追加
            if (startIsOutput && !endIsOutput)
            {
				adjList[startNode].push_back(endNode);
            }
            else
            {
				// 逆向きのリンクが存在する場合は両方のノードを隣接リストに追加
				adjList[endNode].push_back(startNode);
            }
        }
	}

	// ノードの展開状態を管理するためのセット
    std::unordered_set<int> hiddenNodes;

	// ノードを非表示にするためのDFS関数
    std::function<void(int)> dfsHide = [&](int nodeID)
        {
            // 子ノードがいない場合は終了
            if (adjList.find(nodeID) == adjList.end()) return;

            // 子ノードをすべて非表示にする
            for (int childID : adjList[nodeID])
            {
                if (hiddenNodes.find(childID) == hiddenNodes.end())
                {
                    hiddenNodes.insert(childID);
                    dfsHide(childID);
                }
            }
        };

	// 折りたたまれているノードの子ノードをすべて非表示にする
    for (const auto& node : nodes_)
    {
        if (node.isCollapsed)
            dfsHide(node.id);
    }

	// ノードの位置を更新する必要があるかどうかを示すフラグをリセットする
    for (auto& node : nodes_)
    {
        if (hiddenNodes.count(node.id) > 0)
            node.needSetPos = true;
    }


    // ノードエディタの開始
    ImNode::SetCurrentEditor(nodeEditorContext_);
	ImNode::Begin("Behavior Tree Editor Canvas");

    // ノードの描画
    for (auto& node : nodes_)
    {
		// ノードが非表示のセットに含まれている場合は描画をスキップする
		if (hiddenNodes.count(node.id) > 0) continue;

		// ノードの位置をImNodesに反映する必要がある場合は、SetNodeGridSpacePosを呼び出して位置を更新する
        if (node.needSetPos)
        {
            ImNode::SetNodePosition(node.id, ImVec2(node.pos.x, node.pos.y));
            node.needSetPos = false;
        }

        // ノードの開始
        ImNode::BeginNode(node.id);


		// セレクタノードとシーケンスノードの場合は、折りたたみ/展開のトグルボタンを描画する
        if (node.type == EditorNodeType::PersistentSelector || node.type == EditorNodeType::RestartingSelector ||
            node.type == EditorNodeType::PersistentSequence || node.type == EditorNodeType::RestartingSequence)
        {
			if (ImGui::ArrowButton(node.isCollapsed ? "+" : "-", node.isCollapsed ? ImGuiDir_Right : ImGuiDir_Down))
			{
				// ノードの展開状態をトグルする
                node.isCollapsed = !node.isCollapsed;
                history_->SaveHistory(nodes_, links_, currentId_);
			}
        }

        if (node.type == EditorNodeType::PersistentSelector) ImGui::TextUnformatted("Persistent Selector");
        if (node.type == EditorNodeType::PersistentSequence) ImGui::TextUnformatted("Persistent Sequence");
        if (node.type == EditorNodeType::RestartingSelector) ImGui::TextUnformatted("Restarting Selector");
        if (node.type == EditorNodeType::RestartingSequence) ImGui::TextUnformatted("Restarting Sequence");
        if (node.type == EditorNodeType::Condition) ImGui::TextUnformatted("Condition");
        if (node.type == EditorNodeType::Action)ImGui::TextUnformatted("Action");
        
        ImGui::TextUnformatted("Persistent Selector");
        ImGui::Dummy(ImVec2(0, 4)); // 少し余白を空ける
        ImGui::Separator();         // 線で区切る


        // 入力ピンの描画
        ImNode::BeginPin(node.inputPinId, ImNode::PinKind::Input);
        ImGui::Text("In");
        ImNode::EndPin();

		// ノードの内容の描画
        DrawNodeContent(node);

        // ノードの終了
        ImNode::EndNode();

		// ノードの位置をImNodesから取得してノードデータに保存する
        ImVec2 currentPos = ImNode::GetNodePosition(node.id);
        if (currentPos.x > -99999.0f && currentPos.y > -99999.0f) 
        {
            node.pos.x = currentPos.x;
            node.pos.y = currentPos.y;
        }
    }

    // リンクの描画
    for (auto& link : links_)
    {
		// リンクの開始ピンと終了ピンからノードIDと出力ピンか入力ピンかを取得する
		bool startIsOutput = false;
		int startNode = getNodeFromPin(link.startPinId, startIsOutput);

		// リンクの開始ピンと終了ピンからノードIDと出力ピンか入力ピンかを取得する
		bool endIsOutput = false;
		int endNode = getNodeFromPin(link.endPinId, endIsOutput);

		// どちらも有効なノードIDが取得できない場合は描画をスキップする
		if (hiddenNodes.count(startNode) > 0 || hiddenNodes.count(endNode) > 0)continue;

		// リンクを描画する
        ImNode::Link(link.id, link.startPinId, link.endPinId);
    }

    // --- リンクの作成処理 ---
    if (ImNode::BeginCreate())
    {
        ImNode::PinId start_pin, end_pin;
        if (ImNode::QueryNewLink(&start_pin, &end_pin))
        {
            if (start_pin && end_pin)
            {
                if (ImNode::AcceptNewItem())
                {
                    history_->SaveHistory(nodes_, links_, currentId_);
                    EditorLink new_link;
                    new_link.id = GetNextId();
                    new_link.startPinId = static_cast<int>(start_pin.Get());
                    new_link.endPinId = static_cast<int>(end_pin.Get());
                    links_.push_back(new_link);
                }
            }
        }
    }
    ImNode::EndCreate();

    // --- リンク（またはノード）の削除処理 ---
    if (ImNode::BeginDelete())
    {
        ImNode::LinkId deleted_link_id;
        if (ImNode::QueryDeletedLink(&deleted_link_id))
        {
            if (ImNode::AcceptDeletedItem())
            {
                history_->SaveHistory(nodes_, links_, currentId_);
                int link_id = static_cast<int>(deleted_link_id.Get());
                auto it = std::find_if(links_.begin(), links_.end(),
                    [link_id](const EditorLink& link) { return link.id == link_id; });
                if (it != links_.end()) {
                    links_.erase(it);
                }
            }
        }
    }
    ImNode::EndDelete();

    // ノードエディタの終了
    ImNode::End();
    ImNode::SetCurrentEditor(nullptr);

    ImGui::End();

	// 現在のフレームで展開されているノードのIDを更新する
    prevHiddenNodes_ = hiddenNodes;
}

/// @brief ノードの内容を描画する
/// @param node 
void BehaviorTreeEditor::DrawNodeContent(EditorNode& node)
{
    // 条件ノードの場合は関数選択UIを描画
    if (node.type == EditorNodeType::Condition)
    {
		// 条件の種類に応じてUIを切り替える
        switch(node.conditionType)
        {
            // 設定なし
        case ConditionType::None:
            ImGui::Text("None");
            break;

			// ターゲットがいるかどうか
		case ConditionType::HasTarget:
			ImGui::Text("Has Target");
			break;
        }
    }

    // 出力ピンの描画（条件ノード以外）
    if (node.type == EditorNodeType::PersistentSelector || node.type == EditorNodeType::PersistentSequence ||
        node.type == EditorNodeType::RestartingSelector || node.type == EditorNodeType::RestartingSequence)
    {
        ImNode::BeginPin(node.outputPinId, ImNode::PinKind::Output);

        // 出力ピンを右側に配置するためにインデントを追加
        ImGui::Indent(60);
        ImGui::Text("Out");
        ImNode::EndPin();
    }

    // アクションノードの場合はアクション選択UIを描画
    if (node.type == EditorNodeType::Action)
    {
		ImGui::Text("%s", node.actionName.c_str());
    }
}

/// @brief 条件ノードの設定UIを描画する
/// @param node 
void BehaviorTreeEditor::DrawCondtionNodeSettings(EditorNode& node)
{
    ImGui::Text("Function:");

    // コンボボックスに表示する文字列の配列（Enumの順番と一致させる必要があります）
    const char* conditionNames[] = { "None", "HasTarget", "IsTargetDown", "IsNotTargetDown", "IsGrabbing", "IsNotGrabbing", "IsTargetInRange", "IsTargetOutOfRange" };

    // コンボボックスを描画し、変更があったらEnumにキャストして戻す
    int currentItem = static_cast<int>(node.conditionType);
    ImGui::PushItemWidth(120.0f);
    if (ImGui::Combo("Condition", &currentItem, conditionNames, IM_ARRAYSIZE(conditionNames)))
    {
        node.conditionType = static_cast<ConditionType>(currentItem);
    }

    // ターゲットとの距離を条件にする場合は、距離の入力UIを表示
    if (node.conditionType == ConditionType::IsTargetInRange || node.conditionType == ConditionType::IsTargetOutOfRange)
    {
        ImGui::DragFloat("Distance to Target", &node.conditionParam.distanceToTarget, 0.01f, 0.0f, 10000.0f);
    }
}

/// @brief アクションノードの設定UIを描画する
/// @param node 
void BehaviorTreeEditor::DrawActionNodeSettings(EditorNode& node)
{
    ImGui::PushItemWidth(120.0f);

    // アクション選択用のコンボボックス
    const char* actionTypes[] = { "None", "ComboAttack", "GrabAttack", "GrabStrikeAttack", "RequestToken", "ReleaseToken" };
    int currentItem = 0;
    for (int i = 0; i < IM_ARRAYSIZE(actionTypes); ++i)
    {
        if (node.actionName == actionTypes[i])
        {
            currentItem = i;
            break;
        }
    }

    // コンボボックスを描画し、変更があったら選択された文字列をノードに保存
    if (ImGui::Combo("##ActionType", &currentItem, actionTypes, IM_ARRAYSIZE(actionTypes)))
    {
        // 選択された文字列をノードに保存
        node.actionName = actionTypes[currentItem];
    }

    // 選択されているアクションに応じてパラメータ設定UIを切り替える
    if (node.actionName == "ComboAttack")
    {
        if (ImGui::TreeNode("Combo Attack Settings"))
        {
            // DragFloatを使うと、マウスをドラッグして直感的に数値を調整できます (0.01fは変化の速度)
            ImGui::DragFloat("Attack Time", &node.comboAttackInitData.attackTime, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Move Speed", &node.comboAttackInitData.moveSpeed, 0.1f);
            ImGui::DragFloat("Move Start", &node.comboAttackInitData.moveStartTime, 0.01f);
            ImGui::DragFloat("Move End", &node.comboAttackInitData.moveEndTime, 0.01f);

            ImGui::Text("Hitboxes (Multiple)");

            // 当たり判定のリストを描画
            auto& hitDefs = node.comboAttackInitData.hitDefinitions;
            for (size_t i = 0; i < hitDefs.size(); ++i)
            {
                // 各当たり判定のUIを描画するためにIDをプッシュ
                ImGui::PushID(static_cast<int>(i));

                if (ImGui::TreeNode((std::string("Hitbox ") + std::to_string(i + 1)).c_str()))
                {
                    ImGui::DragFloat("Start Time", &hitDefs[i].startTime, 0.01f);
                    ImGui::DragFloat("End Time", &hitDefs[i].endTime, 0.01f);
                    ImGui::DragFloat("Radius", &hitDefs[i].radius, 0.01f);
                    ImGui::InputInt("Damage", &hitDefs[i].damage);
                    ImGui::DragFloat("Knockback", &hitDefs[i].knockback, 0.1f);
                    ImGui::DragFloat3("Knockback Direction", &hitDefs[i].knockbackDirection.x, 0.1f);

                    // ノーマライズされた方向ベクトルを維持するために、ドラッグ後にベクトルを正規化
                    hitDefs[i].knockbackDirection = hitDefs[i].knockbackDirection.Normalize();

                    // ダメージリアクション
                    const char* damageReactionNames[] = { "None", "LightStagger", "HeavyStagger", "Down" };
                    int currentReaction = static_cast<int>(hitDefs[i].damageReaction);
                    if (ImGui::Combo("Damage Reaction", &currentReaction, damageReactionNames, IM_ARRAYSIZE(damageReactionNames))) {
                        hitDefs[i].damageReaction = static_cast<DamageReaction>(currentReaction);
                    }

                    // ジョイントタイプ
                    const char* jointNames[] = { "None","Root","Spine","Chest","Neck","Head","ArmL","ArmR","HandL","HandR","LegL","LegR","FootL","FootR","Weapon" };
                    int currentJoint = static_cast<int>(hitDefs[i].jointType);
                    if (ImGui::Combo("Joint Type", &currentJoint, jointNames, IM_ARRAYSIZE(jointNames))) {
                        hitDefs[i].jointType = static_cast<JointType>(currentJoint);
                    }

                    // 削除ボタン
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                    if (ImGui::Button("Delete Hitbox"))
                    {
                        hitDefs.erase(hitDefs.begin() + i);
                        ImGui::PopStyleColor();
                        ImGui::TreePop();
                        ImGui::PopID();
                        break; // 要素を削除した場合はループを抜ける（次のフレームで再描画される）
                    }
                    ImGui::PopStyleColor();

                    ImGui::TreePop();
                }
                ImGui::PopID();
            }

            // 新しい判定を追加するボタン
            if (ImGui::Button("Add Hitbox"))
            {
                HitboxDefinition newDef;
                // 必要ならデフォルト値を設定
                hitDefs.push_back(newDef);
            }

            ImGui::TreePop();
        }
    }
    else if (node.actionName == "GrabAttack")
    {
        if (ImGui::TreeNode("Grab Attack Settings"))
        {
            ImGui::DragFloat("Attack Time", &node.grabAttackInitData.attackTime, 0.01f);
            ImGui::DragFloat("Grab Time", &node.grabAttackInitData.grabTime, 0.01f);
            ImGui::DragFloat("Move Speed", &node.grabAttackInitData.moveSpeed, 0.1f);
            ImGui::DragFloat("Move Start", &node.grabAttackInitData.moveStartTime, 0.01f);
            ImGui::DragFloat("Move End", &node.grabAttackInitData.moveEndTime, 0.01f);
            ImGui::DragFloat("Hitbox Start", &node.grabAttackInitData.hitboxStartTime, 0.01f);
            ImGui::DragFloat("Hitbox End", &node.grabAttackInitData.hitboxEndTime, 0.01f);

            // ジョイントタイプ
            const char* jointNames[] = { "None","Root","Spine","Chest","Neck","Head","ArmL","ArmR","HandL","HandR","LegL","LegR","FootL","FootR" };
            int currentJoint = static_cast<int>(node.grabAttackInitData.jointType);
            if (ImGui::Combo("Joint Type", &currentJoint, jointNames, IM_ARRAYSIZE(jointNames)))
            {
                node.grabAttackInitData.jointType = static_cast<JointType>(currentJoint);
            }

            ImGui::TreePop();
        }
    }
    else if (node.actionName == "GrabStrikeAttack")
    {
        if (ImGui::TreeNode("Grab Strike Attack Settings"))
        {
            ImGui::DragFloat("Attack Time", &node.grabStrikeAttackInitData.attackTime, 0.01f);
            ImGui::DragFloat("Move Speed", &node.grabStrikeAttackInitData.moveSpeed, 0.1f);
            ImGui::DragFloat("Move Start", &node.grabStrikeAttackInitData.moveStartTime, 0.01f);
            ImGui::DragFloat("Move End", &node.grabStrikeAttackInitData.moveEndTime, 0.01f);
            ImGui::DragFloat("Knockback", &node.grabStrikeAttackInitData.knockback, 0.1f);
            ImGui::DragFloat3("Knockback Direction", &node.grabStrikeAttackInitData.knockbackDirection.x, 0.1f);
            ImGui::Checkbox("Release", &node.grabStrikeAttackInitData.isRelease);

            // 離すタイミングの入力は、isReleaseがtrueの場合にのみ表示
            if (node.grabStrikeAttackInitData.isRelease)
                ImGui::DragFloat("Release Time", &node.grabStrikeAttackInitData.releaseTime, 0.01f);

            // ノーマライズされた方向ベクトルを維持するために、ドラッグ後にベクトルを正規化
            node.grabStrikeAttackInitData.knockbackDirection = node.grabStrikeAttackInitData.knockbackDirection.Normalize();

            // ダメージリアクション
            const char* damageReactionNames[] = { "None", "LightStagger", "HeavyStagger", "Down" };
            int currentReaction = static_cast<int>(node.grabStrikeAttackInitData.damageReaction);
            if (ImGui::Combo("Damage Reaction", &currentReaction, damageReactionNames, IM_ARRAYSIZE(damageReactionNames)))
            {
                node.grabStrikeAttackInitData.damageReaction = static_cast<DamageReaction>(currentReaction);
            }


            // モーションタイプ選択用のコンボボックス
            const char* typeNames[] = { "Stand", "Stance", "Walk", "Dash", "Attack", "Avoid", "Stagger", "Grab", "Grabbed",
                "DownFall", "DownLying", "DowoGetUp", "Guard","styleChange", "grabStrikeAttacker", "grabStrikeTarget" };
            int currentType = static_cast<int>(node.targetMotionType);

            // ターゲットのモーションタイプ選択用のコンボボックス
            if (ImGui::TreeNode("Target Motion"))
            {
                // コンボボックスを描画し、変更があったらEnumにキャストして戻す
                if (ImGui::Combo("Motion Type", &currentType, typeNames, IM_ARRAYSIZE(typeNames)))
                {
                    node.targetMotionType = static_cast<MotionType>(currentType);
                    node.targetMotionName = "";
                }

                // 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
                std::vector<std::string> motionNames = MotionManager::GetInstance()->GetMotionNames(node.targetMotionType);

                // モーション名のリストが空の場合はエラーメッセージを表示
                if (motionNames.empty())
                {
                    ImGui::TextColored(ImVec4(1, 0, 0, 1), "No motions loaded.");
                }
                else
                {
                    // 現在選択されているモーション名をプレビュー用の文字列として設定
                    const char* previewValue = node.targetMotionName.empty() ? "Select Motion..." : node.targetMotionName.c_str();

                    // モーション名選択用のコンボボックスを描画
                    if (ImGui::BeginCombo("Motion Name", previewValue))
                    {
                        for (const auto& name : motionNames)
                        {
                            // 現在のモーション名と同じものが選択されている状態にする
                            bool isSelected = (node.targetMotionName == name);
                            if (ImGui::Selectable(name.c_str(), isSelected))
                            {
                                node.targetMotionName = name;
                            }
                            if (isSelected) ImGui::SetItemDefaultFocus();
                        }
                        ImGui::EndCombo();
                    }
                }

                ImGui::TreePop();
            }


            // ジョイントタイプ
            const char* jointNames[] = { "None","Root","Spine","Chest","Neck","Head","ArmL","ArmR","HandL","HandR","LegL","LegR","FootL","FootR" };
            auto& hits = node.grabStrikeAttackInitData.hits;
            for (size_t i = 0; i < hits.size(); ++i)
            {
                ImGui::PushID(static_cast<int>(i));
                if (ImGui::TreeNode((std::string("Hit ") + std::to_string(i + 1)).c_str()))
                {
                    ImGui::DragFloat("Hit Time", &hits[i].hitTime, 0.01f);
                    ImGui::InputInt("Damage", &hits[i].damage);

                    // ターゲットのジョイントタイプ
                    int targetJoint = static_cast<int>(node.grabAttackInitData.jointType);
                    if (ImGui::Combo("Joint Type", &targetJoint, jointNames, IM_ARRAYSIZE(jointNames)))
                    {
                        node.grabStrikeAttackInitData.hits[i].targetHitJoint = static_cast<JointType>(targetJoint);
                    }
                }
            }

            ImGui::TreePop();
        }
    }

    // None RequestToken ReleaseToke 以外のアクションが選択されている場合はモーション設定UIを表示
    if (node.actionName != "None" && node.actionName != "RequestToken" && node.actionName != "ReleaseToken")
    {
        if (ImGui::TreeNode("Motion Settings"))
        {
            // モーションタイプ選択用のコンボボックス
            const char* typeNames[] = { "Stand", "Stance", "Walk", "Dash", "Attack", "Avoid", "Stagger", "Grab", "Grabbed",
                "DownFall", "DownLying", "DowoGetUp", "Guard", "styleChange", "grabStrikeAttacker", "grabStrikeTarget" };
            int currentType = static_cast<int>(node.motionType);

            // コンボボックスを描画し、変更があったらEnumにキャストして戻す
            if (ImGui::Combo("Motion Type", &currentType, typeNames, IM_ARRAYSIZE(typeNames)))
            {
                node.motionType = static_cast<MotionType>(currentType);
                node.motionName = "";
            }

            // 選択されたモーションタイプに応じたモーション名のリストをMotionManagerから取得
            std::vector<std::string> motionNames = MotionManager::GetInstance()->GetMotionNames(node.motionType);

            // モーション名のリストが空の場合はエラーメッセージを表示
            if (motionNames.empty())
            {
                ImGui::TextColored(ImVec4(1, 0, 0, 1), "No motions loaded.");
            }
            else
            {
                // 現在選択されているモーション名をプレビュー用の文字列として設定
                const char* previewValue = node.motionName.empty() ? "Select Motion..." : node.motionName.c_str();

                // モーション名選択用のコンボボックスを描画
                if (ImGui::BeginCombo("Motion Name", previewValue))
                {
                    for (const auto& name : motionNames)
                    {
                        // 現在のモーション名と同じものが選択されている状態にする
                        bool isSelected = (node.motionName == name);
                        if (ImGui::Selectable(name.c_str(), isSelected))
                        {
                            node.motionName = name;
                        }
                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }

            ImGui::TreePop();
        }
    }

    ImGui::PopItemWidth();
}