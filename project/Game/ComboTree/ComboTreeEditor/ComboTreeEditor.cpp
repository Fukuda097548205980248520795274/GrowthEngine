#include "ComboTreeEditor.h"
#include "GrowthEngine.h"
#include <unordered_set>

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

    // 現在のウィンドウ（ノードエディタ）の位置とサイズを取得して中央の座標を計算
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();
    ImVec2 centerPos = ImVec2(windowPos.x + windowSize.x * 0.5f, windowPos.y + windowSize.y * 0.5f);

    // ImNodesのAPIを使って、いま見えている画面の中央座標にノードを配置する
    ImNodes::SetNodeScreenSpacePos(node.id, centerPos);

    // 構造体側にも一応初期値としてセットしておく
	node.pos = centerPos;

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
		nodeJson["pos"] = { pos.x, pos.y };

        // パラメータ
        nodeJson["animationName"] = node.motionName.c_str();
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
        node.pos.x = nodeJson["pos"][0];
        node.pos.y = nodeJson["pos"][1];

		node.motionName = nodeJson["animationName"].get<std::string>();
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
            clipboard_.HandleCopy(nodes_, links_);
        }

		// Ctrl + Vで貼り付け
        if (io.KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V))
        {
            clipboard_.HandlePaste(*this);
        }

		// Deleteキー Backspaceキー で選択されたノードを削除
        if (ImNodes::NumSelectedNodes() > 0 && (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace)))
        {
			DeleteSelectedNodes();

		}

		// Ctrl + 右クリック でリンクを削除
        DeleteLink();
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
    std::vector<std::string> fileList = projectManager_.GetFileList();

    // タイルのサイズ設定
    const float thumbnailSize = 64.0f;
    const float padding = 12.0f;
    const float cellSize = thumbnailSize + padding;

    // ウィンドウの利用可能な横幅から、1行に収まる列数を自動計算
    float panelWidth = ImGui::GetContentRegionAvail().x;
    int columnCount = static_cast<int>(panelWidth / cellSize);
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

            // 1. 正方形のボタン領域（ホバー・クリックに反応するのはこの四角形のみ）
            if (ImGui::Button(fileName.c_str(), ImVec2(thumbnailSize, thumbnailSize)))
            {
                currentFileName_ = fileName;
                LoadFromFile("./Assets/Parameter/ComboTree/" + currentFileName_ + ".json");
            }

            if (isSelected)
            {
                ImGui::PopStyleColor(3);
            }

            // 2. ボタンの下に表示する名前
            ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 2.0f);
            ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + thumbnailSize);
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

    ImNodes::BeginNodeEditor();

    for (const auto& node : nodes_)
    {
        // --- ノードの描画開始 ---
        ImNodes::BeginNode(node.id);

        // ノードのタイトルバー（あると見栄えが良いです）
        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(node.name);
        ImNodes::EndNodeTitleBar();

        // 左側：入力ピン
        ImNodes::BeginInputAttribute(node.inputPinId);
        ImGui::Text("In");
        ImNodes::EndInputAttribute();

        ImGui::Spacing();

        // 右側：弱攻撃派生ピン
        ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(100, 200, 250, 255));
        ImNodes::BeginOutputAttribute(node.outputLightPinId);
        ImGui::Indent(40.0f);
        ImGui::Text("Light (弱)");
        ImNodes::EndOutputAttribute();
        ImNodes::PopColorStyle();

        // 右側：強攻撃派生ピン
        ImNodes::PushColorStyle(ImNodesCol_Pin, IM_COL32(250, 100, 100, 255));
        ImNodes::BeginOutputAttribute(node.outputHeavyPinId);
        ImGui::Indent(40.0f);
        ImGui::Text("Heavy (強)");
        ImNodes::EndOutputAttribute();
        ImNodes::PopColorStyle();

        // --- ノードの描画終了 ---
        ImNodes::EndNode();
    }

    // リンクの描画
    for (const auto& link : links_)
    {
        if (link.linkType == 1) {
            ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(100, 200, 250, 255)); // 水色
        }
        else if (link.linkType == 2) {
            ImNodes::PushColorStyle(ImNodesCol_Link, IM_COL32(250, 100, 100, 255)); // 赤色
        }

        ImNodes::Link(link.id, link.startPinId, link.endPinId);

        if (link.linkType != 0) {
            ImNodes::PopColorStyle();
        }
    }

    ImNodes::EndNodeEditor();

    // --- リンクが作成されたときの処理 ---
    // ※判定処理はノード描画ループの外（EndNodeEditorの後）に置くのが安全です。
    int startPin, endPin;
    if (ImNodes::IsLinkCreated(&startPin, &endPin))
    {
        ComboEditorLink link;
        link.id = GetNextId();
        link.startPinId = startPin;
        link.endPinId = endPin;

        // 繋いだ元ピンが弱か強かを判定して保存
        link.linkType = 0;
        for (const auto& node : nodes_)
        {
            if (node.outputLightPinId == startPin) link.linkType = 1;
            else if (node.outputHeavyPinId == startPin) link.linkType = 2;
        }

        links_.push_back(link);
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
            ImGui::InputText("Node Name", node->name, sizeof(node->name));
            ImGui::Separator();

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
            int outLightPin = node->outputLightPinId;
            int outHeavyPin = node->outputHeavyPinId;

            for (auto it = links_.begin(); it != links_.end(); )
            {
                if (it->startPinId == inPin || it->startPinId == outLightPin || it->startPinId == outHeavyPin ||
                    it->endPinId == inPin || it->endPinId == outLightPin || it->endPinId == outHeavyPin)
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