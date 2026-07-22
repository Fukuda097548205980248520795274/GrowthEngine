#include "Node.h"
#include "BehaviorTree/BehaviorTreeEditor/BehaviorTreeProjectManager/BehaviorTreeProjectManager.h"

/// @brief コンストラクタ
Node::Node()
{
	// インスタンスを取得する
	engine_ = GrowthEngine::GetInstance();

#ifdef _DEVELOPMENT

	// デフォルトのノードタイプはアクション
	type_ = EditorNodeType::Action;

#endif
}

/// @brief ノードの更新
/// @return 
Node::State Node::UpdateNode()
{
#ifdef _DEVELOPMENT

	// ブレークポイントが設定されている場合、デバッガを起動する
	if (isBreakpoint_)
	{
		__debugbreak();
		isBreakpoint_ = false; // ブレークポイントをリセット
	}

#endif

	lastState_ = Exec();
	return lastState_;
}

#ifdef _DEVELOPMENT

/// @brief デバッグ用の再帰描画処理
void Node::DrawDebuggerRecursive(float zoom)
{
	if (editorNodeId_ < 0) return;

	// デルタタイムを取得
	float dt = ImGui::GetIO().DeltaTime;

	if (lastState_ == State::Success || lastState_ == State::Failure)
	{
		// 実行されたフレームでフェード情報とタイマーをリセット
		fadeState_ = lastState_;
		fadeTimer_ = kFadeDuration;
	}
	else if (lastState_ == State::None && fadeTimer_ > 0.0f)
	{
		// 非実行状態（None）になったらタイマーを減算
		fadeTimer_ -= dt;
		if (fadeTimer_ < 0.0f) fadeTimer_ = 0.0f;
	}
	else if (lastState_ == State::Running)
	{
		// 実行中の場合は残像を消す
		fadeTimer_ = 0.0f;
	}

	// ノードの背景色を決定
	uint32_t color = IM_COL32(60, 60, 60, 255); // None (未実行): グレー

	if (lastState_ == State::Running)
	{
		color = IM_COL32(30, 180, 30, 255);  // Running: 緑
	} 
	else if (lastState_ == State::Success)
	{
		color = IM_COL32(30, 30, 180, 255);  // Success: 青
	} 
	else if (lastState_ == State::Failure)
	{
		color = IM_COL32(180, 30, 30, 255);  // Failure: 赤
	} 
	else if (fadeTimer_ > 0.0f)
	{
		// 残像の色を計算 (t は 1.0 から 0.0 に減衰)
		float t = fadeTimer_ / kFadeDuration;

		// 目標色（Successなら青、Failureなら赤）
		int targetR = (fadeState_ == State::Success) ? 30 : 180;
		int targetG = 30;
		int targetB = (fadeState_ == State::Success) ? 180 : 30;

		// グレー (60, 60, 60) に向かって線形補間（徐々に色を戻す）
		int r = static_cast<int>(60.0f + (targetR - 60.0f) * t);
		int g = static_cast<int>(60.0f + (targetG - 60.0f) * t);
		int b = static_cast<int>(60.0f + (targetB - 60.0f) * t);

		color = IM_COL32(r, g, b, 255);
	}

	// ノードの背景色を適用
	ImNodes::PushColorStyle(ImNodesCol_NodeBackground, color);
	ImNodes::PushColorStyle(ImNodesCol_NodeBackgroundSelected, color);

	// ノードの位置を設定（ズーム率を考慮）
	ImNodes::SetNodeGridSpacePos(editorNodeId_, ImVec2(pos_.x * zoom, pos_.y * zoom));

	// ノードの描画開始
	ImNodes::BeginNode(editorNodeId_);


	ImNodes::BeginNodeTitleBar();
	// ノードの状態をテキストで表示
	std::string stateText = "";
	if (lastState_ == State::Running)
	{
		stateText = "実行中";
	} else if (lastState_ == State::Success)
	{
		stateText = "成功";
	} else if (lastState_ == State::Failure)
	{
		stateText = "失敗";
	} else if (fadeTimer_ > 0.0f)
	{
		// フェードアウト中（直前の状態）も分かりやすいように表示
		stateText = (fadeState_ == State::Success) ? "成功（Fade）" : "失敗（Fade）";
	}

	ImGui::Text("%s", nodeName_.c_str());

	if (!stateText.empty())ImGui::Text("%s", stateText.c_str());
	else ImGui::Text(" ");
	ImNodes::EndNodeTitleBar();



	// ImGuiのID衝突を避けるため、ノードIDをプッシュする
	ImGui::PushID(editorNodeId_);

	// ズーム率に合わせて余白やチェックボックスのスケールを調整
	ImGui::Dummy(ImVec2(0.0f, 2.0f * zoom));

	// ブレークポイントのチェックボックスを描画
	ImGui::Checkbox("Break", &isBreakpoint_);

	// カスタムノードUIの描画
	DrawCustomNodeUI(zoom);

	ImGui::PopID();


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

#endif