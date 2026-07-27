#include "EditorWorkspaceManager.h"

/// @brief 初期化
/// @param stageEditor 
/// @param behaviorTreeEditor 
void EditorWorkspaceManager::Initialize(StageEditor* stageEditor, BehaviorTreeEditor* behaviorTreeEditor, BehaviorTreeViewer* behaviorTreeViewer,
	ComboTreeEditor* comboTreeEditor, CutsceneEditor* cutsceneEditor, UIEditor* uiEditor, ModelEditor* modelEditor, LightEditor* lightEditor)
{
	// nullptrチェック
	assert(stageEditor);
	assert(behaviorTreeEditor);
	assert(behaviorTreeViewer);
	assert(comboTreeEditor);
	assert(cutsceneEditor);
	assert(uiEditor);
	assert(modelEditor);
	assert(lightEditor);

	// 引数を受け取る
	stageEditor_ = stageEditor;
	behaviorTreeEditor_ = behaviorTreeEditor;
	behaviorTreeViewer_ = behaviorTreeViewer;
	comboTreeEditor_ = comboTreeEditor;
	cutsceneEditor_ = cutsceneEditor;
	uiEditor_ = uiEditor;
	modelEditor_ = modelEditor;
	lightEditor_ = lightEditor;
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

	case WorkspaceType::ComboTreeEditor:
		if (comboTreeEditor_)
		{
			comboTreeEditor_->DrawUI();
		}
		break;

	case WorkspaceType::CutsceneEditor:
		if (cutsceneEditor_)
		{
			cutsceneEditor_->SetActive(true);
			cutsceneEditor_->DrawUI();
		}
		break;

	case WorkspaceType::UIEditor:
		if (uiEditor_)
		{
			uiEditor_->DrawUI();
		}
		break;

	case WorkspaceType::ModelEditor:
		if (modelEditor_)
		{
			modelEditor_->DrawUI();
		}
		break;

	case WorkspaceType::LightEditor:
		if (lightEditor_)
		{
			lightEditor_->DrawUI();
		}
		break;
	}
}

/// @brief ワークスペース切り替え用のタブバーを描画する
void EditorWorkspaceManager::DrawWorkspaceTabBar()
{
#ifdef DEVELOPMENT

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

			if (ImGui::BeginTabItem("Combo Tree Editor"))
			{
				currentWorkspace_ = WorkspaceType::ComboTreeEditor;
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Cutscene Editor"))
			{
				currentWorkspace_ = WorkspaceType::CutsceneEditor;
				ImGui::EndTabItem();
			}
			
			if (ImGui::BeginTabItem("UI Editor"))
			{
				currentWorkspace_ = WorkspaceType::UIEditor;
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Model Editor"))
			{
				currentWorkspace_ = WorkspaceType::ModelEditor;
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Light Editor"))
			{
				currentWorkspace_ = WorkspaceType::LightEditor;
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
		ImGui::EndMainMenuBar();
	}

#endif
}