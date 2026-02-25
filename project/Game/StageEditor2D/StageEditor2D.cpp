#include "StageEditor2D.h"
#include "GrowthEngine.h"

/// @brief 更新処理
void StageEditor2D::Update()
{
    ImGui::Begin("Stage Editor");

    // キャンバスのサイズ
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();
    ImVec2 canvasPos = ImGui::GetCursorScreenPos();

    // DrawList を取得
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    // 背景
    drawList->AddRectFilled(canvasPos,
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(50, 50, 50, 255));

    // グリッド描画
    const float gridSize = 32.0f;
    for (float x = 0; x < canvasSize.x; x += gridSize) {
        drawList->AddLine(
            ImVec2(canvasPos.x + x, canvasPos.y),
            ImVec2(canvasPos.x + x, canvasPos.y + canvasSize.y),
            IM_COL32(80, 80, 80, 255)
        );
    }
    for (float y = 0; y < canvasSize.y; y += gridSize) {
        drawList->AddLine(
            ImVec2(canvasPos.x, canvasPos.y + y),
            ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + y),
            IM_COL32(80, 80, 80, 255)
        );
    }

    // タイル配置例
    ImVec2 tilePos = ImVec2(canvasPos.x + 32, canvasPos.y + 32);
    drawList->AddRectFilled(tilePos,
        ImVec2(tilePos.x + gridSize, tilePos.y + gridSize),
        IM_COL32(200, 100, 100, 255));

    ImGui::End();

}