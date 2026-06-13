#pragma once
#include "GrowthEngine.h"
#include "StageEditor/StageData/StageData.h"

class StageEditorHistory;

class StageEditorGuizmo
{
public:

	/// @brief 更新と描画を行う
    /// @param placementList 
    /// @param selectedIndex 
    /// @param isDirty 
    /// @param history 
    void UpdateAndDraw(std::vector<PlacementData>& placementList, int selectedIndex, bool& isDirty, StageEditorHistory* history);


private:

	// 現在の操作モード
    ImGuizmo::OPERATION currentOperation_ = ImGuizmo::TRANSLATE;

	// ローカルモードかワールドモードか
    ImGuizmo::MODE currentMode_ = ImGuizmo::LOCAL;

	// スナップの使用フラグ
    bool useSnap_ = false;

	// エンジンのインスタンス
	const GrowthEngine* engine_ = GrowthEngine::GetInstance();
};

