#include "EditorWorkspaceManager.h"

/// @brief 初期化
/// @param stageEditor 
/// @param behaviorTreeEditor 
void EditorWorkspaceManager::Initialize(StageEditor* stageEditor, BehaviorTreeEditor* behaviorTreeEditor, BehaviorTreeViewer* behaviorTreeViewer,
    MotionManagerEditor* motionManagerEditor)
{
	// nullptrチェック
	assert(stageEditor);
	assert(behaviorTreeEditor);
    assert(behaviorTreeViewer);
	assert(motionManagerEditor);

	// 引数を受け取る
	stageEditor_ = stageEditor;
	behaviorTreeEditor_ = behaviorTreeEditor;
	behaviorTreeViewer_ = behaviorTreeViewer;
	motionManagerEditor_ = motionManagerEditor;
}

/// @brief UIを描画する
void EditorWorkspaceManager::DrawUI()
{
    // 常に画面上部にワークスペース切り替えタブを描画
    DrawWorkspaceTabBar();

    // 選択中のワークスペースに応じてエディタの描画を切り替える
    switch (currentWorkspace_)
    {
    case WorkspaceType::StageEditor:
        if (stageEditor_)
        {
            stageEditor_->DrawUI();
        }
        break;

    case WorkspaceType::BehaviorTreeEditor:
        if (behaviorTreeEditor_)
        {
            behaviorTreeEditor_->DrawUI();
        }
        break;

    case WorkspaceType::BehaviorTreeViewer:
		if (behaviorTreeViewer_)
		{
			behaviorTreeViewer_->DrawUI();
		}
        break;
    }
}

/// @brief ワークスペース切り替え用のタブバーを描画する
void EditorWorkspaceManager::DrawWorkspaceTabBar()
{
#ifdef _DEVELOPMENT

    // 画面の最上部にメニューバーを描画
    if (ImGui::BeginMainMenuBar())
    {
        // メニューバー内にタブバーを作成
        if (ImGui::BeginTabBar("WorkspaceTabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("None"))
            {
                currentWorkspace_ = WorkspaceType::None;
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Stage Editor"))
            {
                currentWorkspace_ = WorkspaceType::StageEditor;
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Behavior Tree Editor"))
            {
                currentWorkspace_ = WorkspaceType::BehaviorTreeEditor;
                ImGui::EndTabItem();
            }

			if (ImGui::BeginTabItem("Behavior Tree Viewer"))
			{
				currentWorkspace_ = WorkspaceType::BehaviorTreeViewer;
				ImGui::EndTabItem();
			}

            ImGui::EndTabBar();
        }
        ImGui::EndMainMenuBar();
    }

#endif
}