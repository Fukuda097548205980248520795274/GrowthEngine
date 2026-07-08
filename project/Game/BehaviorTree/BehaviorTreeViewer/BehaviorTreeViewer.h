#pragma once

class BehaviorTreeViewer
{
public:

	/// @brief コンストラクタ
	BehaviorTreeViewer() = default;

	/// @brief デストラクタ
	~BehaviorTreeViewer() = default;

	/// @brief ビューアーウィンドウを描画する
	void DrawUI();


private:

	// 現在デバッグ対象に選択されているNPCのインデックス
	int selectedNpcIndex_ = 0;

	// ズーム倍率 (1.0f = 等倍)
	float zoom_ = 1.0f;
};

