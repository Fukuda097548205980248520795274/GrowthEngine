#include "BehaviorTreeEditor.h"
#include "BehaviorTreeFactory/BehaviorTreeFactory.h"

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

	// 変更があったのでフラグを立てる
	isDirty_ = true;
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

	// 変更があったのでフラグを立てる
	isDirty_ = true;
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

	// 変更があったのでフラグを立てる
	isDirty_ = true;
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

	// 変更があったのでフラグを立てる
	isDirty_ = true;
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

	// 変更があったのでフラグを立てる
	isDirty_ = true;
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
	node.actionType = ActionType::ComboAttack; // デフォルトでコンボ攻撃に設定
	nodes_.push_back(node);

	// ノードをウィンドウの中心に配置する
	SetNodeWindowCenter(node);

	// 変更があったのでフラグを立てる
	isDirty_ = true;
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

	// 変更が保存されたのでフラグを下ろす
	isDirty_ = false;
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

	// 読み込んだ瞬間は変更がない状態なのでフラグを下ろす
	isDirty_ = false;
}

/// @brief ノードをウィンドウの中心に配置する
/// @param node 
void BehaviorTreeEditor::SetNodeWindowCenter(const EditorNode& node)
{
	// ノードのIDを保留中の中心配置ノードIDとして保存する
	pendingCenterNodeId_ = node.id;
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

/// @brief UIを描画する
void BehaviorTreeEditor::DrawUI()
{
	DrawProjectWindow();
	DrawNodeTable();
	DrawPropertyWindow();
}

/// @brief 選択されているノードを削除する
void BehaviorTreeEditor::DeleteSelectedNodes()
{
	int numSelectedNodes = ImNodes::NumSelectedNodes();
	int numSelectedLinks = ImNodes::NumSelectedLinks();

	// リンクの単体削除処理（既存の処理があればそのまま、または適宜追加）
	if (numSelectedLinks > 0)
	{
		std::vector<int> selectedLinkIds(numSelectedLinks);
		ImNodes::GetSelectedLinks(selectedLinkIds.data());

		links_.erase(std::remove_if(links_.begin(), links_.end(),
			[&selectedLinkIds](const EditorLink& link) {
				return std::find(selectedLinkIds.begin(), selectedLinkIds.end(), link.id) != selectedLinkIds.end();
			}), links_.end());
	}

	// ノードの削除処理（子孫ノードも巻き込んで削除）
	if (numSelectedNodes > 0)
	{
		// 削除前の状態を履歴に保存（履歴機能があれば）
		// history_->SaveHistory(nodes_, links_, currentId_);

		std::vector<int> selectedNodeIds(numSelectedNodes);
		ImNodes::GetSelectedNodes(selectedNodeIds.data());

		// 探索用のキューと、削除対象のノードIDを格納するセット
		std::vector<int> searchQueue = selectedNodeIds;
		std::unordered_set<int> nodesToDelete;

		// コピー時と同様に子孫ノードを探索
		while (!searchQueue.empty())
		{
			int currentId = searchQueue.back();
			searchQueue.pop_back();

			// 既に削除対象に入っていればスキップ
			if (nodesToDelete.count(currentId) > 0) continue;
			nodesToDelete.insert(currentId);

			// 現在のノードを取得
			auto it = std::find_if(nodes_.begin(), nodes_.end(), [currentId](const EditorNode& n) { return n.id == currentId; });
			if (it != nodes_.end())
			{
				int outPinId = it->outputPinId;

				// リンクを辿って子ノードを探す
				for (const auto& link : links_)
				{
					if (link.startPinId == outPinId)
					{
						int childPinId = link.endPinId;
						auto childIt = std::find_if(nodes_.begin(), nodes_.end(), [childPinId](const EditorNode& n) { return n.inputPinId == childPinId; });

						if (childIt != nodes_.end())
						{
							searchQueue.push_back(childIt->id);
						}
					}
				}
			}
		}

		// 削除対象ノードが持っているすべてのピンIDを収集
		// （これらのピンに繋がっているリンクも削除するため）
		std::unordered_set<int> pinsToDelete;
		for (int nodeId : nodesToDelete)
		{
			auto it = std::find_if(nodes_.begin(), nodes_.end(), [nodeId](const EditorNode& n) { return n.id == nodeId; });
			if (it != nodes_.end())
			{
				pinsToDelete.insert(it->inputPinId);
				pinsToDelete.insert(it->outputPinId);
			}
		}

		// 関連するリンクを削除
		links_.erase(std::remove_if(links_.begin(), links_.end(),
			[&pinsToDelete](const EditorLink& link) {
				// リンクの始点か終点のどちらかが削除対象のピンなら削除
				return pinsToDelete.count(link.startPinId) > 0 || pinsToDelete.count(link.endPinId) > 0;
			}), links_.end());

		// ノード本体を削除
		nodes_.erase(std::remove_if(nodes_.begin(), nodes_.end(),
			[&nodesToDelete](const EditorNode& n) {
				return nodesToDelete.count(n.id) > 0;
			}), nodes_.end());

		// 選択状態をクリア
		ImNodes::ClearNodeSelection();
	}


	// 変更があったのでフラグを立てる
	history_->SaveHistory(nodes_, links_, currentId_);
	isDirty_ = true;
}