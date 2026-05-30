#include "BehaviorTreeEditor.h"
#include "BehaviorTree/BehaviorTreeFactory/BehaviorTreeFactory.h"

/// @brief コンストラクタ
BehaviorTreeEditor::BehaviorTreeEditor()
{
	// 履歴管理クラスとクリップボード管理クラスのインスタンスを作成
	history_ = std::make_unique<BehaviorTreeEditorHistory>();
	clipboard_ = std::make_unique<BehaviorTreeEditorClipboard>();

	// ファイルアイコンのSRVのGPUハンドルを取得
    btFileIcon_ = engine_->GetTextureSrvGpuHandle(engine_->LoadTexture("./Assets/Textures/uvChecker.png"));
}

/// @brief セレクタノードを追加する
void BehaviorTreeEditor::AddPersistentSelectorNode()
{
	// ノード追加前の状態を履歴に保存する
    history_->SaveHistory(nodes_, links_, currentId_);

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
    history_->SaveHistory(nodes_, links_, currentId_);

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
    history_->SaveHistory(nodes_, links_, currentId_);

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
    history_->SaveHistory(nodes_, links_, currentId_);

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
    history_->SaveHistory(nodes_, links_, currentId_);

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
    history_->SaveHistory(nodes_, links_, currentId_);

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

/// @brief エディタを初期状態にリセットする
void BehaviorTreeEditor::ClearEditor()
{
    nodes_.clear();
    links_.clear();
    currentId_ = 1;
    
	// 履歴もクリアする
	history_->Clear();
}

/// @brief 現在のツリー構造をファイルに保存する
void BehaviorTreeEditor::SaveCurrentTree()
{
    if (currentFileName_.empty()) return;

	// 変更を保存する
    saver_.SaveTree(currentFileName_, nodes_, links_);
}

/// @brief ファイルからツリー構造を読み込む
/// @param fileName 
void BehaviorTreeEditor::LoadTree(const std::string& fileName)
{
	// ファイルから読み込む前にエディタを初期状態にリセットする
    ClearEditor();
    currentFileName_ = fileName;
    saver_.LoadTree(fileName, nodes_, links_);

	// ノードの展開状態を管理するマップと前のフレームで展開されていたノードのセットをクリアする
	prevHiddenNodes_.clear();

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
void BehaviorTreeEditor::SetNodeWindowCenter(const EditorNode& node)
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
        if (n.id >= currentId_) currentId_ = n.id + 1;
        if (n.inputPinId >= currentId_) currentId_ = n.inputPinId + 1;
        if (n.outputPinId >= currentId_) currentId_ = n.outputPinId + 1;
    }

	// リンクIDの最大値も確認
    for (const auto& l : links_)
    {
        if (l.id >= currentId_) currentId_ = l.id + 1;
    }

	return BehaviorTreeFactory::CreateTree(nodes, links, character);
}

/// @brief 選択されているノードを削除する
void BehaviorTreeEditor::DeleteSelectedNodes()
{
	// 選択されているノードの数を取得
    int numSelected = ImNodes::NumSelectedNodes();
    if (numSelected <= 0) return;

    // ノード追加前の状態を履歴に保存する
    history_->SaveHistory(nodes_, links_, currentId_);

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

			// ノードに接続されているリンクをすべて削除する
            links_.erase(
                std::remove_if(links_.begin(), links_.end(),
                    [inPin, outPin](const EditorLink& link) {
                        return link.startPinId == outPin || link.endPinId == inPin;
                    }),
                links_.end()
            );

			// ノードを削除
            nodes_.erase(nodeIt);
        }
    }

    // ImNodes側の選択状態もクリアしておく
    ImNodes::ClearNodeSelection();
}