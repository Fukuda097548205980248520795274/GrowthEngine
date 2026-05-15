#include "BehaviorTreeEditor.h"
#include "BehaviorTree/BehaviorTreeFactory/BehaviorTreeFactory.h"

/// @brief セレクタノードを追加する
void BehaviorTreeEditor::AddPersistentSelectorNode()
{
	// ノード追加前の状態を履歴に保存する
    SaveHistory();

    EditorNode node;
    node.id = GetNextId();
    node.type = EditorNodeType::PersistentSelector;
    node.inputPinId = GetNextId();
    node.outputPinId = GetNextId();
    nodes_.push_back(node);

	// ノードをウィンドウの中心に配置する
    SetNodeWindowCenter(node);
}

/// @brief シーケンスノードを追加する
void BehaviorTreeEditor::AddPersistentSequenceNode()
{
    // ノード追加前の状態を履歴に保存する
    SaveHistory();

    EditorNode node;
    node.id = GetNextId();
    node.type = EditorNodeType::PersistentSequence;
    node.inputPinId = GetNextId();
    node.outputPinId = GetNextId();
    nodes_.push_back(node);

    // ノードをウィンドウの中心に配置する
    SetNodeWindowCenter(node);
}

/// @brief セレクタノードを追加する
void BehaviorTreeEditor::AddRestartingSelectorNode()
{
    // ノード追加前の状態を履歴に保存する
    SaveHistory();

    EditorNode node;
    node.id = GetNextId();
    node.type = EditorNodeType::RestartingSelector;
    node.inputPinId = GetNextId();
    node.outputPinId = GetNextId();
    nodes_.push_back(node);

    // ノードをウィンドウの中心に配置する
    SetNodeWindowCenter(node);
}

/// @brief シーケンスノードを追加する
void BehaviorTreeEditor::AddRestartingSequenceNode()
{
    // ノード追加前の状態を履歴に保存する
    SaveHistory();

    EditorNode node;
    node.id = GetNextId();
    node.type = EditorNodeType::RestartingSequence;
    node.inputPinId = GetNextId();
    node.outputPinId = GetNextId();
    nodes_.push_back(node);

    // ノードをウィンドウの中心に配置する
    SetNodeWindowCenter(node);
}

/// @brief 条件ノードを追加する
void BehaviorTreeEditor::AddConditionNode()
{
    // ノード追加前の状態を履歴に保存する
    SaveHistory();

    EditorNode node;
    node.id = GetNextId();
    node.type = EditorNodeType::Condition;
    node.inputPinId = GetNextId();
    node.outputPinId = -1;
    nodes_.push_back(node);

    // ノードをウィンドウの中心に配置する
    SetNodeWindowCenter(node);
}

/// @brief コンボ攻撃ノードを追加する
void BehaviorTreeEditor::AddActionNode()
{
    // ノード追加前の状態を履歴に保存する
    SaveHistory();

    EditorNode node;
	node.id = GetNextId();
	node.type = EditorNodeType::Action;
    node.inputPinId = GetNextId();
	node.outputPinId = -1;
	node.actionName = "None"; // アクション名を設定
	nodes_.push_back(node);

    // ノードをウィンドウの中心に配置する
    SetNodeWindowCenter(node);
}



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
    


	// ノードエディタの開始
    ImNodes::BeginNodeEditor();

	// ノードの描画
    for (auto& node : nodes_)
    {
		// ノードの開始
        ImNodes::BeginNode(node.id);

		// ノードタイトルの描画
        ImNodes::BeginNodeTitleBar();
        if (node.type == EditorNodeType::PersistentSelector) ImGui::TextUnformatted("Persistent Selector");
        if (node.type == EditorNodeType::PersistentSequence) ImGui::TextUnformatted("Persistent Sequence");
        if (node.type == EditorNodeType::RestartingSelector) ImGui::TextUnformatted("Restarting Selector");
        if (node.type == EditorNodeType::RestartingSequence) ImGui::TextUnformatted("Restarting Sequence");
        if (node.type == EditorNodeType::Condition) ImGui::TextUnformatted("Condition");
		if (node.type == EditorNodeType::Action)ImGui::TextUnformatted("Action");
        ImNodes::EndNodeTitleBar();

		// 入力ピンの描画
        ImNodes::BeginInputAttribute(node.inputPinId);
        ImGui::Text("In");
        ImNodes::EndInputAttribute();

		// 条件ノードの場合は関数選択UIを描画
        if (node.type == EditorNodeType::Condition)
        {
            ImGui::Text("Function:");

            // コンボボックスに表示する文字列の配列（Enumの順番と一致させる必要があります）
            const char* conditionNames[] = { "None", "HasTarget", "IsTargetDown", "IsNotTargetDown"};

            // 現在のEnumの値をintにキャスト
            int currentItem = static_cast<int>(node.conditionType);

            // コンボボックスを描画し、変更があったらEnumにキャストして戻す
            ImGui::PushItemWidth(120.0f);
            if (ImGui::Combo("Condition", &currentItem, conditionNames, IM_ARRAYSIZE(conditionNames)))
            {
                node.conditionType = static_cast<ConditionType>(currentItem);
            }
        }

		// 出力ピンの描画（条件ノード以外）
        if (node.type == EditorNodeType::PersistentSelector || node.type == EditorNodeType::PersistentSequence ||
            node.type == EditorNodeType::RestartingSelector || node.type == EditorNodeType::RestartingSequence)
        {
            ImNodes::BeginOutputAttribute(node.outputPinId);
            
			// 出力ピンを右側に配置するためにインデントを追加
            ImGui::Indent(60);
            ImGui::Text("Out");
            ImNodes::EndOutputAttribute();
        }

		// アクションノードの場合はアクション選択UIを描画
        if (node.type == EditorNodeType::Action)
        {
            ImGui::PushItemWidth(120.0f);

            // アクション選択用のコンボボックス
            const char* actionTypes[] = { "None", "ComboAttack", "GrabAttack" };

            // 現在選択されているインデックスを探す
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
                    ImGui::DragFloat("Hitbox Start", &node.comboAttackInitData.hitboxStartTime, 0.01f);
                    ImGui::DragFloat("Hitbox End", &node.comboAttackInitData.hitboxEndTime, 0.01f);
                    ImGui::DragFloat("Cancel Start", &node.comboAttackInitData.cancelStartTime, 0.01f);
                    ImGui::DragFloat("Cancel End", &node.comboAttackInitData.cancelEndTime, 0.01f);
                    ImGui::DragFloat("Knockback", &node.comboAttackInitData.knockback, 0.1f);
					ImGui::DragFloat3("Knockback Direction", &node.comboAttackInitData.knockbackDirection.x, 0.1f);

					// ノックバック方向を正規化
					node.comboAttackInitData.knockbackDirection = node.comboAttackInitData.knockbackDirection.Normalize();

                    // int型の場合は InputInt や DragInt を使用
                    ImGui::InputInt("Damage", &node.comboAttackInitData.damage);

                    // ダメージリアクション
                    const char* damageReactionNames[] = { "None", "LightStagger", "HeavyStagger", "Down"};
                    int currentReaction = static_cast<int>(node.comboAttackInitData.damageReaction);
                    if (ImGui::Combo("Damage Reaction", &currentReaction, damageReactionNames, IM_ARRAYSIZE(damageReactionNames)))
                    {
                        node.comboAttackInitData.damageReaction = static_cast<DamageReaction>(currentReaction);
                    }

					// ジョイントタイプ
                    const char* jointNames[] = { "None","Root","Spine","Chest","Neck","Head","ArmL","ArmR","HandL","HandR","LegL","LegR","FootL","FootR" };
                    int currentJoint = static_cast<int>(node.comboAttackInitData.jointType);
					if (ImGui::Combo("Joint Type", &currentJoint, jointNames, IM_ARRAYSIZE(jointNames)))
                    {
                        node.comboAttackInitData.jointType = static_cast<JointType>(currentJoint);
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

			// アクションがNoneでない場合はモーション設定UIも表示
            if (node.actionName != "None")
            {
				if (ImGui::TreeNode("Motion Settings"))
                {
                    // モーションタイプ選択用のコンボボックス
                    const char* typeNames[] = { "Stand", "Stance", "Walk", "Dash", "Attack", "Avoid", "Stagger", "Grab", "Grabbed", "DownFall", "DownLying", "DowoGetUp", "Guard" };
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
                    } else
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

		// ノードの終了
        ImNodes::EndNode();
    }

	// リンクの描画
    for (auto& link : links_)
    {
        ImNodes::Link(link.id, link.startPinId, link.endPinId);
    }

	// ノードエディタの終了
    ImNodes::EndNodeEditor();


    // コピーとペーストの処理
    HandleCopy();
    HandlePaste();

    // DeleteキーまたはBackspaceキーが押された場合、選択されているノードを削除する
    if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace))
    {
        DeleteSelectedNodes();
    }

    // Ctrl+ZでUndo、Ctrl+YでRedoを実行する
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Z))
    {
        Undo();
    }

    // Ctrl+YでRedoを実行
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_Y))
    {
        Redo();
    }
    


	// リンクの作成と削除の処理
    int start_pin, end_pin;

	// リンクが作成された場合
    if (ImNodes::IsLinkCreated(&start_pin, &end_pin))
    {
        // ノード追加前の状態を履歴に保存する
        SaveHistory();

		// 新しいリンクを追加
        EditorLink new_link;
        new_link.id = GetNextId();
        new_link.startPinId = start_pin;
        new_link.endPinId = end_pin;
        links_.push_back(new_link);
    }


	// リンクが削除された場合
    int link_id;
    
	// ImNodes::IsLinkDestroyedは削除されたリンクのIDを返す関数
    if (ImNodes::IsLinkDestroyed(&link_id))
    {
        // ノード追加前の状態を履歴に保存する
        SaveHistory();

        auto it = std::find_if(links_.begin(), links_.end(),
            [link_id](const EditorLink& link) { return link.id == link_id; });
        if (it != links_.end()) {
            links_.erase(it);
        }
    }

    ImGui::End();

#endif
}

void BehaviorTreeEditor::ClearEditor()
{
    nodes_.clear();
    links_.clear();
    currentId_ = 1;
    
    undoHistory_.clear();
    redoHistory_.clear();
}

void BehaviorTreeEditor::SaveCurrentTree()
{
    if (currentFileName_.empty()) return;

    // 最新の座標を反映
    for (auto& node : nodes_)
    {
        ImVec2 pos = ImNodes::GetNodeGridSpacePos(node.id);
        node.pos.x = pos.x;
        node.pos.y = pos.y;
    }

    saver_.SaveTree(currentFileName_, nodes_, links_);
}

void BehaviorTreeEditor::LoadTree(const std::string& fileName)
{
    ClearEditor();
    currentFileName_ = fileName;
    saver_.LoadTree(fileName, nodes_, links_);

    // IDの最大値を見つけて、次に振るIDが被らないようにする
    for (const auto& n : nodes_)
    {
        if (n.id >= currentId_) currentId_ = n.id + 1;
        if (n.inputPinId >= currentId_) currentId_ = n.inputPinId + 1;
        if (n.outputPinId >= currentId_) currentId_ = n.outputPinId + 1;
    }

    for (const auto& l : links_)
    {
        if (l.id >= currentId_) currentId_ = l.id + 1;
    }

    // ノードの座標をImNodesに適用（次の描画フレームで反映される）
    for (const auto& node : nodes_)
    {
        ImNodes::SetNodeGridSpacePos(node.id, ImVec2(node.pos.x, node.pos.y));
    }
}

/// @brief ノードをウィンドウの中心に配置する
/// @param node 
void BehaviorTreeEditor::SetNodeWindowCenter(EditorNode node)
{
    // ノードをウィンドウの中心に配置する
    ImVec2 windowCenter(ImGui::GetWindowPos().x + ImGui::GetWindowSize().x * 0.5f, ImGui::GetWindowPos().y + ImGui::GetWindowSize().y * 0.5f);

    // ImNodesを使用してノードの位置を設定
    ImNodes::SetNodeScreenSpacePos(node.id, windowCenter);
}

/// @brief エディタ上のノードとリンクからビヘイビアツリーを生成する
/// @param fileName 
/// @return 
std::unique_ptr<BehaviorTree> BehaviorTreeEditor::CreateTree(const std::string& fileName, Character* character)
{
    std::vector<EditorNode> nodes;
    std::vector<EditorLink> links;

	// ファイルからノードとリンクの情報を読み込む
	saver_.LoadTree(fileName, nodes, links);

	// ノードIDとノードのマッピングを作成
    for (const auto& n : nodes_)
    {
        if (n.id >= nextId_) nextId_ = n.id + 1;
        if (n.inputPinId >= nextId_) nextId_ = n.inputPinId + 1;
        if (n.outputPinId >= nextId_) nextId_ = n.outputPinId + 1;
    }

	// リンクIDの最大値も確認
    for (const auto& l : links_)
    {
        if (l.id >= nextId_) nextId_ = l.id + 1;
    }

	return BehaviorTreeFactory::CreateTree(nodes, links, character);
}

/// @brief 選択されているノードを削除する
void BehaviorTreeEditor::DeleteSelectedNodes()
{
    // 1. 選択されているノードの数を取得
    int numSelected = ImNodes::NumSelectedNodes();
    if (numSelected <= 0) return;

    // ノード追加前の状態を履歴に保存する
    SaveHistory();

    // 選択されたノードのIDをすべて取得
    std::vector<int> selectedNodes(numSelected);
    ImNodes::GetSelectedNodes(selectedNodes.data());

    for (int nodeId : selectedNodes)
    {
        // 削除対象のノードを見つける
        auto nodeIt = std::find_if(nodes_.begin(), nodes_.end(),
            [nodeId](const EditorNode& n) { return n.id == nodeId; });

        if (nodeIt != nodes_.end())
        {
            int inPin = nodeIt->inputPinId;
            int outPin = nodeIt->outputPinId;

            // 2. そのノードのピンに繋がっていたリンクをすべて削除
            links_.erase(
                std::remove_if(links_.begin(), links_.end(),
                    [inPin, outPin](const EditorLink& link) {
                        return link.startPinId == outPin || link.endPinId == inPin;
                    }),
                links_.end()
            );

            // 3. ノード自体を削除
            nodes_.erase(nodeIt);
        }
    }

    // ImNodes側の選択状態もクリアしておく
    ImNodes::ClearNodeSelection();
}

/// @brief コピーしたノードとリンクの情報をクリップボードに保存する
void BehaviorTreeEditor::HandleCopy()
{
    // Ctrl + C が押されたか判定
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_C))
    {
        // 選択されているノードの数を取得
        int numSelectedNodes = ImNodes::NumSelectedNodes();
        if (numSelectedNodes > 0)
        {
            // 選択されたノードのIDを取得
            std::vector<int> selectedNodeIds(numSelectedNodes);
            ImNodes::GetSelectedNodes(selectedNodeIds.data());

            clipboardNodes_.clear();
            clipboardLinks_.clear();

            // 1. ノードをクリップボードにコピー
            std::vector<int> copiedPinIds; // リンク判定用にピンIDを記録
            for (int id : selectedNodeIds)
            {
                auto it = std::find_if(nodes_.begin(), nodes_.end(), [id](const EditorNode& n) { return n.id == id; });
                if (it != nodes_.end())
                {
                    clipboardNodes_.push_back(*it);
                    copiedPinIds.push_back(it->inputPinId);
                    copiedPinIds.push_back(it->outputPinId);
                }
            }

            // 2. リンクをクリップボードにコピー
            // 「開始ピン」と「終了ピン」の両方がコピー対象ノードに含まれているリンクのみコピーする
            for (const auto& link : links_)
            {
                bool startInCopied = std::find(copiedPinIds.begin(), copiedPinIds.end(), link.startPinId) != copiedPinIds.end();
                bool endInCopied = std::find(copiedPinIds.begin(), copiedPinIds.end(), link.endPinId) != copiedPinIds.end();

                if (startInCopied && endInCopied)
                {
                    clipboardLinks_.push_back(link);
                }
            }
        }
    }
}

/// @brief コピーしたノードとリンクの情報をクリップボードに保存する
void BehaviorTreeEditor::HandlePaste()
{
    // Ctrl + V が押されたか判定
    if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V))
    {
        if (clipboardNodes_.empty()) return;

        // ノード追加前の状態を履歴に保存する
        SaveHistory();

        // ペースト時に既存の選択状態を解除する
        ImNodes::ClearNodeSelection();
        ImNodes::ClearLinkSelection();

        // コピー元のピンID -> 新しく生成したピンID の変換マップ
        std::unordered_map<int, int> oldToNewPinId;
        std::vector<int> newlyAddedNodeIds;

		// マウスの位置
        ImVec2 mouseScreenPos = ImGui::GetMousePos();

        // クリップボード内のノード群の「左上の座標」を計算（マウス位置に一番左上のノードを合わせるため）
        float minX = 999999.0f;
        float minY = 999999.0f;
        for (const auto& clipNode : clipboardNodes_)
        {
            if (clipNode.pos.x < minX) minX = clipNode.pos.x;
            if (clipNode.pos.y < minY) minY = clipNode.pos.y;
        }

        // 1. ノードのペースト
        for (const auto& clipNode : clipboardNodes_)
        {
            EditorNode newNode = clipNode; // 種類や設定値をそのままコピー

            // 新しいIDを発行
            newNode.id = GetNextId();
            newNode.inputPinId = GetNextId();
            newNode.outputPinId = GetNextId();

            // リンク復元のために新旧ピンIDの対応を記録
            oldToNewPinId[clipNode.inputPinId] = newNode.inputPinId;
            oldToNewPinId[clipNode.outputPinId] = newNode.outputPinId;

            // グリッド座標系での「相対距離（オフセット）」を計算
            float offsetX = clipNode.pos.x - minX;
            float offsetY = clipNode.pos.y - minY;

            nodes_.push_back(newNode);
            newlyAddedNodeIds.push_back(newNode.id);

			// ノードの位置をマウス位置に合わせて設定
            ImVec2 targetScreenPos(mouseScreenPos.x + offsetX, mouseScreenPos.y + offsetY);
            ImNodes::SetNodeScreenSpacePos(newNode.id, targetScreenPos);
        }

        // 2. リンクのペースト
        for (const auto& clipLink : clipboardLinks_)
        {
            EditorLink newLink;
            newLink.id = GetNextId();

            // 変換マップを使って、新しく生成したピン同士をつなぐ
            newLink.startPinId = oldToNewPinId[clipLink.startPinId];
            newLink.endPinId = oldToNewPinId[clipLink.endPinId];

            links_.push_back(newLink);
        }

        // 3. ペーストしたばかりのノードを選択状態にする（そのままドラッグで動かせるように）
        for (int id : newlyAddedNodeIds)
        {
            ImNodes::SelectNode(id);
        }
    }
}

/// @brief 現在の状態を履歴に保存する
void BehaviorTreeEditor::SaveHistory()
{
    // 最新のグリッド座標をImNodesから取得してノードデータに反映
    for (auto& node : nodes_)
    {
        ImVec2 pos = ImNodes::GetNodeGridSpacePos(node.id);
        node.pos.x = pos.x;
        node.pos.y = pos.y;
    }

    // 現在の状態をスナップショットとして保存
    EditorSnapshot snapshot;
    snapshot.nodes = nodes_;
    snapshot.links = links_;
    snapshot.currentId = currentId_;

    undoHistory_.push_back(snapshot);
    redoHistory_.clear(); // 新しい操作が行われたらRedo履歴は破棄する

    // 履歴の最大数を制限（メモリ節約のため、例として50回まで）
    if (undoHistory_.size() > 50)
    {
        undoHistory_.erase(undoHistory_.begin());
    }
}

/// @brief Undo（元に戻す）を実行する
void BehaviorTreeEditor::Undo()
{
    if (undoHistory_.empty()) return;

    // 現在の状態をRedo履歴に保存しておく
    for (auto& node : nodes_)
    {
        ImVec2 pos = ImNodes::GetNodeGridSpacePos(node.id);
        node.pos.x = pos.x;
        node.pos.y = pos.y;
    }
    EditorSnapshot currentSnapshot = { nodes_, links_, currentId_ };
    redoHistory_.push_back(currentSnapshot);

    // Undo履歴の最後尾から状態を復元
    EditorSnapshot snapshot = undoHistory_.back();
    undoHistory_.pop_back();

    nodes_ = snapshot.nodes;
    links_ = snapshot.links;
    currentId_ = snapshot.currentId;

    // 復元した座標をImNodesに反映
    for (const auto& node : nodes_)
    {
        ImNodes::SetNodeGridSpacePos(node.id, ImVec2(node.pos.x, node.pos.y));
    }
    ImNodes::ClearNodeSelection();
    ImNodes::ClearLinkSelection();
}

/// @brief Redo（やり直す）を実行する
void BehaviorTreeEditor::Redo()
{
    if (redoHistory_.empty()) return;

    // 現在の状態をUndo履歴に保存しておく
    for (auto& node : nodes_)
    {
        ImVec2 pos = ImNodes::GetNodeGridSpacePos(node.id);
        node.pos.x = pos.x;
        node.pos.y = pos.y;
    }
    EditorSnapshot currentSnapshot = { nodes_, links_, currentId_ };
    undoHistory_.push_back(currentSnapshot);

    // Redo履歴の最後尾から状態を復元
    EditorSnapshot snapshot = redoHistory_.back();
    redoHistory_.pop_back();

    nodes_ = snapshot.nodes;
    links_ = snapshot.links;
    currentId_ = snapshot.currentId;

    // 復元した座標をImNodesに反映
    for (const auto& node : nodes_)
    {
        ImNodes::SetNodeGridSpacePos(node.id, ImVec2(node.pos.x, node.pos.y));
    }
    ImNodes::ClearNodeSelection();
    ImNodes::ClearLinkSelection();
}