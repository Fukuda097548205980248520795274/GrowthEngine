#pragma once
#include "GrowthEngine.h"
#include "BehaviorTree/BehaviorTreeEditor/BehaviorTreeEditor.h"
#include "BehaviorTree/BehaviorTreeViewer/BehaviorTreeViewer.h"
#include "StageEditor/StageEditor.h"
#include "ComboTree/ComboTreeEditor/ComboTreeEditor.h"
#include "MotionManager/MotionManagerEditor/MotionManagerEditor.h"

enum class WorkspaceType
{
	None,
	StageEditor,
	BehaviorTreeEditor,
	BehaviorTreeViewer,
	ComboTreeEditor,
};

class EditorWorkspaceManager
{
public:

	/// @brief 初期化
	/// @param stageEditor 
	/// @param behaviorTreeEditor 
	/// @param motionManagerEditor 
	void Initialize(StageEditor* stageEditor, BehaviorTreeEditor* behaviorTreeEditor, BehaviorTreeViewer* behaviorTreeViewer,
		ComboTreeEditor* comboTreeEditor, MotionManagerEditor* motionManagerEditor);

	/// @brief UIを描画する
	void DrawUI();

private:

	/// @brief ワークスペース切り替え用のタブバーを描画する
	void DrawWorkspaceTabBar();

	WorkspaceType currentWorkspace_ = WorkspaceType::StageEditor;

	// マネージャー側は指し示すポインタ（生ポインタ）だけを持つ
	StageEditor* stageEditor_ = nullptr;
	BehaviorTreeEditor* behaviorTreeEditor_ = nullptr;
	BehaviorTreeViewer* behaviorTreeViewer_ = nullptr;
	ComboTreeEditor* comboTreeEditor_ = nullptr;
	MotionManagerEditor* motionManagerEditor_ = nullptr;

	// エディタ間で共有するデータなど
	std::vector<PlacementData> placementList_;
	std::string currentStageFileName_;
};

