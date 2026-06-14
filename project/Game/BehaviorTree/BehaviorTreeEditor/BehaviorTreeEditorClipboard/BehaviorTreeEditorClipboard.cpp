#include "BehaviorTreeEditorClipboard.h"
#include "../BehaviorTreeEditor.h"

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
	}
}

/// @brief コピーしたノードとリンクの情報をエディタにペーストする
/// @param editor 
void BehaviorTreeEditorClipboard::HandlePaste(BehaviorTreeEditor& editor)
{
	// Ctrl + V が押されたか判定
	if (ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_V))
	{
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

			editor.links_.push_back(newLink);
		}

		// ペーストしたノードを選択状態にする
		for (int id : newlyAddedNodeIds)
		{
			ImNodes::SelectNode(id);
		}
	}
}