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

    int selectedNpcIndex_ = 0; // 現在デバッグ対象に選択されているNPCのインデックス
};

