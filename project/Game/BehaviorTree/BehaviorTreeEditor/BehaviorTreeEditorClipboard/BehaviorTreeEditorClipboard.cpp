#include "BehaviorTreeEditorClipboard.h"
#include "../BehaviorTreeEditor.h"
#include "SharedTreeEditorClipboard/SharedTreeEditorClipboard.h"

/// @brief コピーしたノードとリンクの情報をクリップボードに保存する
/// @param sourceNodes 
/// @param sourceLinks 
void BehaviorTreeEditorClipboard::HandleCopy(const std::vector<EditorNode>& sourceNodes, const std::vector<EditorLink>& sourceLinks)
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

			// 選択されたノードとその全ての子孫ノードのIDを格納するリスト
			std::vector<int> allNodeIdsToCopy;
			std::vector<int> searchQueue = selectedNodeIds;
			std::unordered_set<int> visitedNodes; // 重複探索を防止

			// 子孫ノードを探索してコピー対象を収集する
			while (!searchQueue.empty())
			{
				int currentId = searchQueue.back();
				searchQueue.pop_back();

				// 既にチェック済みのノードならスキップ
				if (visitedNodes.count(currentId) > 0) continue;
				visitedNodes.insert(currentId);
				allNodeIdsToCopy.push_back(currentId);

				// 現在のノード情報を取得
				auto it = std::find_if(sourceNodes.begin(), sourceNodes.end(), [currentId](const EditorNode& n) { return n.id == currentId; });
				if (it != sourceNodes.end())
				{
					int outPinId = it->outputPinId;

					// このノードの outputPinId を開始点(startPinId)とするリンクを探す
					for (const auto& link : sourceLinks)
					{
						if (link.startPinId == outPinId)
						{
							// リンクの終点(endPinId)を持つ子ノードを探す
							int childPinId = link.endPinId;
							auto childIt = std::find_if(sourceNodes.begin(), sourceNodes.end(), [childPinId](const EditorNode& n) { return n.inputPinId == childPinId; });

							// 子ノードが見つかったら探索キューに追加
							if (childIt != sourceNodes.end())
							{
								searchQueue.push_back(childIt->id);
							}
						}
					}
				}
			}

			// 収集したすべてのノードとピンIDを記録
			std::vector<int> copiedPinIds;
			for (int id : allNodeIdsToCopy)
			{
				auto it = std::find_if(sourceNodes.begin(), sourceNodes.end(), [id](const EditorNode& n) { return n.id == id; });
				if (it != sourceNodes.end())
				{
					clipboardNodes_.push_back(*it);
					copiedPinIds.push_back(it->inputPinId);
					copiedPinIds.push_back(it->outputPinId);
				}
			}

			// コピーしたノードのピンIDをもとに、該当するリンクをクリップボードに保存
			for (const auto& link : sourceLinks)
			{
				bool startInCopied = std::find(copiedPinIds.begin(), copiedPinIds.end(), link.startPinId) != copiedPinIds.end();
				bool endInCopied = std::find(copiedPinIds.begin(), copiedPinIds.end(), link.endPinId) != copiedPinIds.end();

				// 親子ともコピー対象に含まれていればリンクもコピー
				if (startInCopied && endInCopied)
				{
					clipboardLinks_.push_back(link);
				}
			}
		}

		// 共有クリップボードへも保存
		auto& shared = SharedTreeEditorClipboard::GetInstance();
		shared.Clear();
		shared.currentDataType = SharedTreeEditorClipboard::DataType::BehaviorTree;
		shared.behaviorNodes = clipboardNodes_;
	}
}

/// @brief コピーしたノードとリンクの情報をエディタにペーストする
/// @param editor 
void BehaviorTreeEditorClipboard::HandlePaste(BehaviorTreeEditor& editor)
{
	// Ctrl + V が押されたか判定
	if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V))
	{
		auto& shared = SharedTreeEditorClipboard::GetInstance();

		// 共有クリップボードにコンボツリーのデータがある場合は、そちらを優先してペーストする
		if (shared.currentDataType == SharedTreeEditorClipboard::DataType::ComboTree)
		{
			if (shared.comboNodes.empty()) return;

			// ノード追加前の状態を履歴に保存する
			editor.history_->SaveHistory(editor.nodes_, editor.links_, editor.currentId_);

			// ペースト時に既存の選択状態を解除する
			ImNodes::ClearNodeSelection();
			ImNodes::ClearLinkSelection();

			std::vector<int> newlyAddedNodeIds;
			ImVec2 mouseScreenPos = ImGui::GetMousePos();

			// 配置用にクリップボード内の左上座標を計算
			float minX = 999999.0f;
			float minY = 999999.0f;
			for (const auto& cNode : shared.comboNodes)
			{
				if (cNode.pos.x < minX) minX = cNode.pos.x;
				if (cNode.pos.y < minY) minY = cNode.pos.y;
			}

			// コンボツリーのノードをビヘイビアツリーのアクションノードに変換
			for (const auto& cNode : shared.comboNodes)
			{
				EditorNode bNode;
				
				bNode.name[0] = '\0';
				bNode.isCollapsed = false;
				bNode.needSetPos = true;

				// リンクは繋がないので、新しくIDを発行するだけ
				bNode.id = editor.GetNextId();
				bNode.inputPinId = editor.GetNextId();
				bNode.outputPinId = editor.GetNextId();

				// ノードタイプはすべてアクションノードにする
				bNode.type = EditorNodeType::Action;

				// ノードの種類に応じてActionTypeとパラメータをマッピング
				if (cNode.nodeType == ComboNodeType::Combo)
				{
					bNode.actionType = ActionType::ComboAttack;
					bNode.comboAttackInitData = cNode.comboAttackInitData;
				}
				else if (cNode.nodeType == ComboNodeType::Grab)
				{
					bNode.actionType = ActionType::GrabAttack;
					bNode.grabAttackInitData = cNode.grabAttackInitData;
				}
				else if (cNode.nodeType == ComboNodeType::GrabStrike)
				{
					bNode.actionType = ActionType::GrabStrikeAttack;
					bNode.grabStrikeAttackInitData = cNode.grabStrikeAttackInitData;
				}
				else
				{
					// 万が一未知のタイプが来たらスキップ
					continue;
				}

				bNode.motionName = cNode.motionName;
				bNode.targetMotionName = cNode.targetMotionName;

				// ノードの位置をマウス位置に合わせて相対配置
				float offsetX = cNode.pos.x - minX;
				float offsetY = cNode.pos.y - minY;
				bNode.pos = Vector2(mouseScreenPos.x + offsetX, mouseScreenPos.y + offsetY);

				editor.nodes_.push_back(bNode);
				newlyAddedNodeIds.push_back(bNode.id);

				// ImNodesに座標をセット
				ImNodes::SetNodeScreenSpacePos(bNode.id, ImVec2(bNode.pos.x, bNode.pos.y));
			}

			// ペーストしたノードを選択状態にする
			for (int id : newlyAddedNodeIds)
			{
				ImNodes::SelectNode(id);
			}

			editor.isDirty_ = true; // 変更フラグを立てる

			return; // コンボツリーからのペーストが完了したらここで終了
		}


		if (clipboardNodes_.empty()) return;

		// ノード追加前の状態を履歴に保存する
		editor.history_->SaveHistory(editor.nodes_, editor.links_, editor.currentId_);

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

		// クリップボード内のノード情報をもとに、エディタのノードリストに新しいノードを追加する
		for (const auto& clipNode : clipboardNodes_)
		{
			EditorNode newNode = clipNode; // 種類や設定値をそのままコピー

			// 新しいIDを発行
			newNode.id = editor.GetNextId();
			newNode.inputPinId = editor.GetNextId();
			newNode.outputPinId = editor.GetNextId();

			// リンク復元のために新旧ピンIDの対応を記録
			oldToNewPinId[clipNode.inputPinId] = newNode.inputPinId;
			oldToNewPinId[clipNode.outputPinId] = newNode.outputPinId;

			// グリッド座標系での「相対距離（オフセット）」を計算
			float offsetX = clipNode.pos.x - minX;
			float offsetY = clipNode.pos.y - minY;

			editor.nodes_.push_back(newNode);
			newlyAddedNodeIds.push_back(newNode.id);

			// ノードの位置をマウス位置に合わせて設定
			ImVec2 targetScreenPos(mouseScreenPos.x + offsetX, mouseScreenPos.y + offsetY);
			ImNodes::SetNodeScreenSpacePos(newNode.id, targetScreenPos);
		}

		// クリップボード内のリンク情報をもとに、エディタのリンクリストに新しいリンクを追加する
		for (const auto& clipLink : clipboardLinks_)
		{
			EditorLink newLink;
			newLink.id = editor.GetNextId();

			// 変換マップを使って、新しく生成したピン同士をつなぐ
			newLink.startPinId = oldToNewPinId[clipLink.startPinId];
			newLink.endPinId = oldToNewPinId[clipLink.endPinId];

			// ピンIDからノードIDを取得して保存
			newLink.startNodeId = editor.GetNodeIdFromPinId(newLink.startPinId);
			newLink.endNodeId = editor.GetNodeIdFromPinId(newLink.endPinId);

			editor.links_.push_back(newLink);
		}

		// ペーストしたノードを選択状態にする
		for (int id : newlyAddedNodeIds)
		{
			ImNodes::SelectNode(id);
		}
	}
}