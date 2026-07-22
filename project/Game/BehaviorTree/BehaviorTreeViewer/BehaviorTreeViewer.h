#pragma once

class NPC;

class BehaviorTreeViewer
{
public:

	/// @brief コンストラクタ
	BehaviorTreeViewer() = default;

	/// @brief デストラクタ
	~BehaviorTreeViewer() = default;

	/// @brief UIを描画する
	void DrawUI();


private:

	// 選択されたNPCのインデックス
	NPC* selectedNpc_ = nullptr;

	// ズーム倍率 (1.0f = 等倍)
	float zoom_ = 1.0f;
};

