#include "Node.h"
#include "BehaviorTree/BehaviorTreeEditor/BehaviorTreeProjectManager/BehaviorTreeProjectManager.h"

/// @brief コンストラクタ
Node::Node()
{
	// インスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

	// デフォルトのノードタイプはアクション
	type_ = EditorNodeType::Action;
}

/// @brief ノードの更新
/// @return 
Node::State Node::UpdateNode()
{
	lastState_ = Exec();
	return lastState_;
}

/// @brief デバッグ用の再帰描画処理
void Node::DrawDebuggerRecursive(float zoom)
{
	if (editorNodeId_ < 0) return;

	// 現在の実行状態（lastState_）に応じてノードの背景色を決定
	uint32_t color = IM_COL32(60, 60, 60, 255); // None (未実行): グレー
	switch (lastState_)
	{
	case State::Running:
		color = IM_COL32(30, 180, 30, 255);  // Running: 緑
		break;
	case State::Success:
		color = IM_COL32(30, 30, 180, 255);  // Success: 青
		break;
	case State::Failure:
		color = IM_COL32(180, 30, 30, 255);  // Failure: 赤
		break;
	default:
		break;
	}

	// ノードの背景色を適用
	ImNodes::PushColorStyle(ImNodesCol_NodeBackground, color);
	ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected, color);

	// ノードの位置を設定（ズーム率を考慮）
	ImNodes::SetNodeGridSpacePos(editorNodeId_, ImVec2(pos_.x * zoom, pos_.y * zoom));

	// ノードの描画開始
	ImNodes::BeginNode(editorNodeId_);

	ImNodes::BeginNodeTitleBar();
	ImGui::Text("%s", nodeName_.c_str());
	ImNodes::EndNodeTitleBar();

	ImGui::Dummy(ImVec2(50.0f * zoom, 0.0f));

	// 入力ピンの描画（ピンIDが有効な場合のみ）
	if (inputPinId_ > 0)
	{
		ImNodes::BeginInputAttribute(inputPinId_);
		ImGui::Text("In");
		ImNodes::EndInputAttribute();
	}

	// 出力ピンの描画（アクション以外、かつピンIDが有効な場合のみ）
	if (type_ != EditorNodeType::Action && outputPinId_ > 0)
	{
		ImNodes::BeginOutputAttribute(outputPinId_);
		ImGui::Text("Out");
		ImNodes::EndOutputAttribute();
	}

	ImNodes::EndNode();

	ImNodes::PopColorStyle();
	ImNodes::PopColorStyle();
}

/// @brief デバッグ情報を設定する
/// @param id 
/// @param inPin 
/// @param outPin 
/// @param pos 
/// @param name 
/// @param type 
void Node::SetDebugInfo(int id, int inPin, int outPin, const Vector2& pos, const std::string& name, EditorNodeType type)
{
	editorNodeId_ = id;
	inputPinId_ = inPin;
	outputPinId_ = outPin;
	pos_ = pos;
	nodeName_ = name;
	type_ = type;
}