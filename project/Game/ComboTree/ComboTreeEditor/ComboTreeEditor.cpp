#include "ComboTreeEditor.h"
#include "GrowthEngine.h"

using json = nlohmann::json;

/// @brief ノードを追加する
void ComboTreeEditor::AddComboAttackNode()
{
	// 新しいノードを作成
    ComboEditorNode node;
    node.id = GetNextId();
    node.inputPinId = GetNextId();
    node.outputLightPinId = GetNextId();
    node.outputHeavyPinId = GetNextId();

    // 初期配置座標
    node.posX = 100.0f;
    node.posY = 100.0f;

	// ノードを配列に追加
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
        nodeJson["name"] = node.name;
        nodeJson["inputPinId"] = node.inputPinId;
        nodeJson["outputLightPinId"] = node.outputLightPinId;
        nodeJson["outputHeavyPinId"] = node.outputHeavyPinId;

        // エディタ上の最新のノード座標を取得して保存
        ImVec2 pos = ImNodes::GetNodeGridSpacePos(node.id);
        nodeJson["posX"] = pos.x;
        nodeJson["posY"] = pos.y;

        // パラメータ
        nodeJson["animationName"] = node.animationName;
        nodeJson["attackTime"] = node.attackTime;
        nodeJson["moveSpeed"] = node.moveSpeed;
        nodeJson["moveStartTime"] = node.moveStartTime;
        nodeJson["moveEndTime"] = node.moveEndTime;
        nodeJson["cancelStartTime"] = node.cancelStartTime;
        nodeJson["cancelEndTime"] = node.cancelEndTime;

        // 当たり判定配列の保存
        nodeJson["hitDefinitions"] = json::array();
        for (const auto& def : node.hitDefinitions)
        {
            json defJson;
            defJson["startTime"] = def.startTime;
            defJson["endTime"] = def.endTime;
            defJson["damage"] = def.damage;
            defJson["radius"] = def.radius;
            defJson["knockback"] = def.knockback;
            defJson["knockbackDirection"] = { def.knockbackDirection.x, def.knockbackDirection.y, def.knockbackDirection.z };
            defJson["damageReaction"] = static_cast<int>(def.damageReaction);
            defJson["jointType"] = static_cast<int>(def.jointType); // 必要に応じて追加
            nodeJson["hitDefinitions"].push_back(defJson);
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
        node.id = nodeJson["id"];
        strcpy_s(node.name, nodeJson["name"].get<std::string>().c_str());
        node.inputPinId = nodeJson["inputPinId"];
        node.outputLightPinId = nodeJson["outputLightPinId"];
        node.outputHeavyPinId = nodeJson["outputHeavyPinId"];
        node.posX = nodeJson["posX"];
        node.posY = nodeJson["posY"];

        strcpy_s(node.animationName, nodeJson["animationName"].get<std::string>().c_str());
        node.attackTime = nodeJson["attackTime"];
        node.moveSpeed = nodeJson["moveSpeed"];
        node.moveStartTime = nodeJson["moveStartTime"];
        node.moveEndTime = nodeJson["moveEndTime"];
        node.cancelStartTime = nodeJson["cancelStartTime"];
        node.cancelEndTime = nodeJson["cancelEndTime"];

        // 当たり判定配列の復元
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

                auto targetDir = defJson["knockbackDirection"];
                def.knockbackDirection = Vector3(targetDir[0], targetDir[1], targetDir[2]);
                def.damageReaction = static_cast<DamageReaction>(defJson["damageReaction"].get<int>());
                def.jointType = static_cast<JointType>(defJson["jointType"].get<int>());

                node.hitDefinitions.push_back(def);
            }
        }

        nodes_.push_back(node);

        // ImNodesに座標を即座に反映させる（重要）
        ImNodes::SetNodeGridSpacePos(node.id, ImVec2(node.posX, node.posY));
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
	DrawNodeEditor();
	DrawPropertyPanel();
}

/// @brief ノードエディタを描画する
void ComboTreeEditor::DrawNodeEditor()
{
    // テスト用にノードを追加するボタン
    if (ImGui::Button("Add Combo Node"))
    {
        AddComboAttackNode();
    }

    if (ImGui::Button("Save"))
    {
        SaveToFile("./Assets/Parameter/ComboTree/test_combo.json"); // 保存先パスは環境に合わせてください
    }
    ImGui::SameLine();
    if (ImGui::Button("Load"))
    {
        LoadFromFile("./Assets/Parameter/ComboTree/test_combo.json");
    }

    // ImNodesの配置開始
    ImNodes::BeginNodeEditor();

    for (const auto& node : nodes_)
    {
        // ノードの開始
        ImNodes::BeginNode(node.id);

        // 1. ノードのタイトル
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(node.name);
        ImNodes::EndNodeTitleBar();

        // 2. ピンの配置（ImGuiのLayoutを使って左右に分ける）
        // 左側：入力ピン
        ImNodes::BeginInputAttribute(node.inputPinId);
        ImGui::Text("In");
        ImNodes::EndInputAttribute();

        ImGui::Spacing();

        // 右側：弱攻撃派生ピン
        ImNodes::BeginOutputAttribute(node.outputLightPinId);
        ImGui::Indent(40.0f); // 右側に寄せるためのインデント
        ImGui::Text("Light (弱)");
        ImNodes::EndOutputAttribute();

        // 右側：強攻撃派生ピン
        ImNodes::BeginOutputAttribute(node.outputHeavyPinId);
        ImGui::Indent(40.0f);
        ImGui::Text("Heavy (強)");
        ImNodes::EndOutputAttribute();

        // ノードの終了
        ImNodes::EndNode();


        // 選択されているノードを取得
        int numSelectedNodes = ImNodes::NumSelectedNodes();
        if (numSelectedNodes > 0)
        {
            std::vector<int> selectedNodes(numSelectedNodes);
            ImNodes::GetSelectedNodes(selectedNodes.data());
            selectedNodeId_ = selectedNodes[0]; // まずは単一選択のみ対応
        }
        else
        {
            selectedNodeId_ = -1; // 選択解除
        }
    }

    // リンクの描画（これがないと繋いだ線が表示されません）
    for (const auto& link : links_)
    {
        ImNodes::Link(link.id, link.startPinId, link.endPinId);
    }

    ImNodes::EndNodeEditor();

    // --- リンクの結合処理（ドラッグ＆ドロップされたとき） ---
    int startPin, endPin;
    if (ImNodes::IsLinkCreated(&startPin, &endPin))
    {
        ComboEditorLink link;
        link.id = GetNextId();
        link.startPinId = startPin;
        link.endPinId = endPin;
        links_.push_back(link);
    }
}

/// @brief プロパティパネルを描画する
void ComboTreeEditor::DrawPropertyPanel()
{
    // プロパティウィンドウの描画
    ImGui::Begin("Combo Properties");

    if (selectedNodeId_ != -1)
    {
        ComboEditorNode* node = GetNodeById(selectedNodeId_);
        if (node)
        {
            ImGui::InputText("Node Name", node->name, sizeof(node->name));
            ImGui::Separator();

            ImGui::Text("Attack Parameters");
            ImGui::InputText("Animation", node->animationName, sizeof(node->animationName));

            // パラメータの編集 (DragFloatを使ってスライダー式にするのがおすすめです)
            ImGui::DragFloat("Attack Time", &node->attackTime, 0.01f, 0.0f, 10.0f);

            ImGui::Spacing();
            ImGui::Text("Movement");
            ImGui::DragFloat("Move Speed", &node->moveSpeed, 0.1f, 0.0f, 100.0f);
            ImGui::DragFloat("Move Start Time", &node->moveStartTime, 0.01f, 0.0f, node->attackTime);
            ImGui::DragFloat("Move End Time", &node->moveEndTime, 0.01f, 0.0f, node->attackTime);

            ImGui::Spacing();
            ImGui::Text("Cancel Window");
            ImGui::DragFloat("Cancel Start Time", &node->cancelStartTime, 0.01f, 0.0f, node->attackTime);
            ImGui::DragFloat("Cancel End Time", &node->cancelEndTime, 0.01f, 0.0f, node->attackTime);


            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Text("Hitboxes (当たり判定)");

            // 新しい当たり判定を追加するボタン
            if (ImGui::Button("Add Hitbox"))
            {
                node->hitDefinitions.push_back(HitboxDefinition());
            }

            // 配列の各要素を描画
            for (int i = 0; i < node->hitDefinitions.size(); ++i)
            {
                // ImGuiのID衝突を避けるためにインデックスでPushする
                ImGui::PushID(i);

                // 折りたたみ可能なツリーノードでまとめる
                char hitboxName[32];
                sprintf_s(hitboxName, "Hitbox [%d]", i);
                if (ImGui::TreeNode(hitboxName))
                {
                    HitboxDefinition& def = node->hitDefinitions[i];

                    ImGui::DragFloat("Start Time", &def.startTime, 0.01f, 0.0f, node->attackTime);
                    ImGui::DragFloat("End Time", &def.endTime, 0.01f, 0.0f, node->attackTime);
                    ImGui::DragInt("Damage", &def.damage, 1, 0, 9999);
                    ImGui::DragFloat("Radius", &def.radius, 0.01f, 0.0f, 10.0f);
                    ImGui::DragFloat("Knockback", &def.knockback, 0.1f, 0.0f, 100.0f);
                    ImGui::DragFloat3("Knockback Dir", &def.knockbackDirection.x, 0.01f);

                    // 列挙型 (DamageReaction) のコンボボックス
                    // 怯みなし, 小怯み, 大怯み, ダウン, 受け流され, 弾かれ を選択できるようにする
                    const char* reactionNames[] = { "None", "LightStagger", "HeavyStagger", "Down", "Deflected", "Repelled" };
                    int currentReaction = static_cast<int>(def.damageReaction);
                    if (ImGui::Combo("Reaction", &currentReaction, reactionNames, IM_ARRAYSIZE(reactionNames)))
                    {
                        def.damageReaction = static_cast<DamageReaction>(currentReaction);
                    }

                    // ※ jointType も同様に Combo を使って文字列の配列から選ばせるのがオススメです。
                    // (例: "HandR", "HandL", "FootR", "FootL" など)

                    ImGui::Spacing();

                    // 削除ボタン
                    // 削除ボタンを赤色にする（見た目の工夫）
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
                    if (ImGui::Button("Delete Hitbox"))
                    {
                        node->hitDefinitions.erase(node->hitDefinitions.begin() + i);
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
    else
    {
        ImGui::Text("ノードが選択されていません");
    }

    ImGui::End();
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