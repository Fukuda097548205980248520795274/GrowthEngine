#include "MotionManagerEditor.h"
#include "../MotionManager.h"

/// @brief エディタの描画
void MotionManagerEditor::Draw()
{
#ifdef _DEVELOPMENT

    ImGui::Begin("Motion Manager Editor");

    auto* motionManager = MotionManager::GetInstance();
    const auto& table = motionManager->GetMotionTable();

    // 読み込まれているすべてのモーションをツリー表示
    for (const auto& [type, motionMap] : table) 
    {
        // Enumから文字列への変換は、ヘルパー関数を別途用意すると見栄えが良くなります
        std::string typeName = "Type: " + std::to_string(static_cast<int>(type));

        if (ImGui::TreeNode(typeName.c_str()))
        {
            for (const auto& [name, handle] : motionMap)
            {
                ImGui::Text("Key: [%s]  ->  Handle ID: %d", name.c_str(), handle); // handleの中身に合わせて表示形式は調整してください

                // ※ここにプレビュー再生用のボタンなどを配置するとさらに便利です
                // if (ImGui::Button(("Play##" + name).c_str())) { ... }
            }
            ImGui::TreePop();
        }
    }

    ImGui::End();

#endif
}