#include "MotionManagerEditor.h"
#include "../MotionManager.h"

/// @brief エディタの描画
void MotionManagerEditor::Draw()
{
#ifdef DEVELOPMENT

    ImGui::Begin("Motion Manager Editor");

    auto* motionManager = MotionManager::GetInstance();
    const auto& table = motionManager->GetMotionTable();

    // 読み込まれているすべてのモーションをツリー表示
    for (const auto& [type, motionMap] : table) 
    {
        std::string typeName = "Type: " + std::to_string(static_cast<int>(type));

        if (ImGui::TreeNode(typeName.c_str()))
        {
            for (const auto& [name, handle] : motionMap)
            {
                ImGui::Text("Key: [%s]  ->  Handle ID: %d", name.c_str(), handle);
            }
            ImGui::TreePop();
        }
    }

    ImGui::End();

#endif
}