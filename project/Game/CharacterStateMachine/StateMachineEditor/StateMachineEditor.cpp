#include "StateMachineEditor.h"
#include "Entity/Character/Character.h"

/// @brief UIを描画する
/// @param character 
void StateMachineEditor::DrawUI(Character* character)
{
    if (!character) return;

    // ステートマシンを取得する
    CharacterStateMachine* stateMachine = character->GetStateMachine();
    if (!stateMachine) return;

    ImGui::Begin("ステートマシンエディタ");

	// 現在のステート名とステート一覧を取得する
    const std::string& currentStateName = stateMachine->GetCurrentStateName();
    const auto& states = stateMachine->GetStates();

    // 2カラムレイアウト（左: ステート一覧, 右: 選択ステートの詳細設定）
    ImGui::Columns(2, "StateMachineColumns", true);

    
	// 左カラム: ステート一覧
    ImGui::Text("States (%d)", static_cast<int>(states.size()));
    ImGui::Separator();

    ImGui::BeginChild("StateListChild", ImVec2(0, 0), true);
    for (const auto& [name, state] : states)
    {
        bool isCurrent = (name == currentStateName);
        bool isSelected = (name == selectedStateName_);

        // 現在実行中のステートは緑色で表示
        if (isCurrent)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 1.0f, 0.2f, 1.0f));
        }

        std::string label = name;
        if (isCurrent) label += " [Active]";

        // ステートの選択
        if (ImGui::Selectable(label.c_str(), isSelected))
        {
            selectedStateName_ = name;
        }

        if (isCurrent)
        {
            ImGui::PopStyleColor();
        }
    }
    ImGui::EndChild();

    
	// 右カラム: 選択ステートの詳細設定
    ImGui::NextColumn();
    ImGui::Text("State Details");
    ImGui::Separator();

    if (!selectedStateName_.empty() && states.find(selectedStateName_) != states.end())
    {
        ImGui::Text("Selected: %s", selectedStateName_.c_str());

        // デバッグ用: 任意でステートを強制切り替え
        if (ImGui::Button("Force Transition To This State"))
        {
            stateMachine->ChangeState(selectedStateName_);
        }

        ImGui::Spacing();
        ImGui::Separator();

		// ビヘイビアツリーやコンボツリーの設定エリア
        ImGui::TextDisabled("ビヘイビアツリー / コンボツリー 設定");
    }
    else
    {
        ImGui::TextDisabled("左のリストからステートを選択してください。");
    }

    ImGui::Columns(1);
    ImGui::End();
}