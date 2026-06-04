#include "EditorWorkspaceManager.h"

/// @brief 初期化
/// @param stageEditor 
/// @param behaviorTreeEditor 
void EditorWorkspaceManager::Initialize(StageEditor* stageEditor, BehaviorTreeEditor* behaviorTreeEditor, MotionManagerEditor* motionManagerEditor)
{
	// nullptrチェック
	assert(stageEditor);
	assert(behaviorTreeEditor);
	assert(motionManagerEditor);

	// 引数を受け取る
	stageEditor_ = stageEditor;
	behaviorTreeEditor_ = behaviorTreeEditor;
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
    }
}

/// @brief ワークスペース切り替え用のタブバーを描画する
void EditorWorkspaceManager::DrawWorkspaceTabBar()
{
    // 画面の最上部にメニューバーを描画
    if (ImGui::BeginMainMenuBar())
    {
        // メニューバー内にタブバーを作成
        if (ImGui::BeginTabBar("WorkspaceTabs", ImGuiTabBarFlags_None))
        {
            if (ImGui::BeginTabItem("Stage Editor"))
            {
                currentWorkspace_ = WorkspaceType::StageEditor;
                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Behavior Tree"))
            {
                currentWorkspace_ = WorkspaceType::BehaviorTreeEditor;
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }
        ImGui::EndMainMenuBar();
    }
}