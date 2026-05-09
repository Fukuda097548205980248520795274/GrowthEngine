#include "BehaviorTreeEditor.h"

/// @brief セレクタノードを追加する
void BehaviorTreeEditor::AddPersistentSelectorNode()
{
    EditorNode node;
    node.id = GetNextId();
    node.type = EditorNodeType::PersistentSelector;
    node.inputPinId = GetNextId();
    node.outputPinId = GetNextId();
    nodes_.push_back(node);
}

/// @brief シーケンスノードを追加する
void BehaviorTreeEditor::AddPersistentSequenceNode()
{
    EditorNode node;
    node.id = GetNextId();
    node.type = EditorNodeType::PersistentSequence;
    node.inputPinId = GetNextId();
    node.outputPinId = GetNextId();
    nodes_.push_back(node);
}

/// @brief セレクタノードを追加する
void BehaviorTreeEditor::AddRestartingSelectorNode()
{
    EditorNode node;
    node.id = GetNextId();
    node.type = EditorNodeType::RestartingSelector;
    node.inputPinId = GetNextId();
    node.outputPinId = GetNextId();
    nodes_.push_back(node);
}

/// @brief シーケンスノードを追加する
void BehaviorTreeEditor::AddRestartingSequenceNode()
{
    EditorNode node;
    node.id = GetNextId();
    node.type = EditorNodeType::RestartingSequence;
    node.inputPinId = GetNextId();
    node.outputPinId = GetNextId();
    nodes_.push_back(node);
}

/// @brief 条件ノードを追加する
void BehaviorTreeEditor::AddConditionNode()
{
    EditorNode node;
    node.id = GetNextId();
    node.type = EditorNodeType::Condition;
    node.inputPinId = GetNextId();
    node.outputPinId = -1;
    nodes_.push_back(node);
}

/// @brief ノードテーブルを描画する
void BehaviorTreeEditor::DrawNodeTable()
{
#ifdef _DEVELOPMENT

    ImGui::Begin("Behavior Tree Editor");

    // --- 1. 新規作成 ---
    if (ImGui::Button("New Tree"))
    {
        ImGui::OpenPopup("NewTreePopup");
    }

    if (ImGui::BeginPopup("NewTreePopup"))
    {
        static char newFileName[64] = "";
        ImGui::InputText("File Name", newFileName, 64);
        if (ImGui::Button("Create"))
        {
            currentFileName_ = newFileName;
            ClearEditor(); // 既存のノードとリンクを全削除
            SaveCurrentTree(); // 空の状態で一度保存
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }

    ImGui::SameLine();

    // --- 2. ファイル一覧とロード ---
    ImGui::Text("Saved Trees:");
    auto files = projectManager_.GetFileList();

    for (const auto& file : files)
    {
        // 現在開いているファイルはハイライトする
        bool isSelected = (currentFileName_ == file);
        if (ImGui::Selectable(file.c_str(), isSelected))
        {
            LoadTree(file);
        }
    }

    ImGui::SameLine();

    // --- 3. 上書き保存 ---
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
        ImNodes::EndNodeTitleBar();

		// 入力ピンの描画
        ImNodes::BeginInputAttribute(node.inputPinId);
        ImGui::Text("In");
        ImNodes::EndInputAttribute();

		// 条件ノードの場合は関数選択UIを描画
        if (node.type == EditorNodeType::Condition)
        {
            ImGui::Text("Function:");

            // 実際はここで実行する関数をコンボボックス等で選ばせる
            // ImGui::InputText("##func", ...);
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
    


	// リンクの作成と削除の処理
    int start_pin, end_pin;

	// リンクが作成された場合
    if (ImNodes::IsLinkCreated(&start_pin, &end_pin))
    {
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
    nextId_ = 1;
    // ImNodesの内部状態（キャンバス位置など）もリセット可能
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
        if (n.id >= nextId_) nextId_ = n.id + 1;
        if (n.inputPinId >= nextId_) nextId_ = n.inputPinId + 1;
        if (n.outputPinId >= nextId_) nextId_ = n.outputPinId + 1;
    }

    for (const auto& l : links_)
    {
        if (l.id >= nextId_) nextId_ = l.id + 1;
    }

    // ノードの座標をImNodesに適用（次の描画フレームで反映される）
    for (const auto& node : nodes_)
    {
        ImNodes::SetNodeGridSpacePos(node.id, ImVec2(node.pos.x, node.pos.y));
    }
}