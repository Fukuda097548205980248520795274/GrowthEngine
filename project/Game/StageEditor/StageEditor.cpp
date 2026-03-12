#include "StageEditor.h"
#include "GrowthEngine.h"
#include "StageSerializer/StageSerializer.h"

/// @brief エディターUIの更新
void StageEditor::UpdateEditorUI(StageSerializer* serializer)
{
#ifdef _DEVELOPMENT


	// リサイズ設定
	ImGui::Begin("Stage Editor");
	if (ImGui::Button("Resize")) { isResize_ = true; }

	// サイズがなかったら処理しない
	if (stageData_.width_ > 0 && stageData_.height_ > 0)
	{
		ImGui::Text("\n");

		// 保存
		if (ImGui::Button("Save"))
		{
			serializer->SaveCSV(stageData_);
		}

		ImGui::Text("\n");

		// 読み込み
		static char fileName[24];
		ImGui::InputText("fileName", fileName, IM_ARRAYSIZE(fileName));
		if (ImGui::Button("Load"))
		{
			serializer->LoadCSV(stageData_, fileName);
		}

		ImGui::Text("\n");

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

		for (int y = 0; y < stageData_.height_; y++)
		{
			for (int x = 0; x < stageData_.width_; x++)
			{
				// 何もないときは描画しない
				if (stageData_.tiles_[y][x] == StageData::Tile::Air)
					continue;

				ImU32 color;
				switch (stageData_.tiles_[y][x])
				{
				case StageData::Tile::Wall:
					color = IM_COL32(200, 100, 100, 255);
					break;

				case StageData::Tile::Player:
					color = IM_COL32(200, 10, 10, 255);
					break;
				}

				ImVec2 pMin = ImVec2(canvasPos.x + x * gridSize,
					canvasPos.y + y * gridSize);
				ImVec2 pMax = ImVec2(pMin.x + gridSize,
					pMin.y + gridSize);

				drawList->AddRectFilled(pMin, pMax, color);

			}
		}



		// ===============================
		// ★ カーソル位置のハイライト処理
		// ===============================
		ImVec2 mousePos = ImGui::GetMousePos();

		// キャンバス内にいるか？
		bool inside =
			mousePos.x >= canvasPos.x &&
			mousePos.x < canvasPos.x + canvasSize.x &&
			mousePos.y >= canvasPos.y &&
			mousePos.y < canvasPos.y + canvasSize.y;

		static int lastPaintX = -1;
		static int lastPaintY = -1;

		if (inside) {
			int cellX = (int)((mousePos.x - canvasPos.x) / gridSize);
			int cellY = (int)((mousePos.y - canvasPos.y) / gridSize);

			// 左クリック押しっぱなしで連続配置
			if (ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
				if (cellX >= 0 && cellX < stageData_.width_ &&
					cellY >= 0 && cellY < stageData_.height_) {

					// 同じマスを連続で塗らないようにする
					if (cellX != lastPaintX || cellY != lastPaintY) {
						stageData_.tiles_[cellY][cellX] = selectedTile_;
						lastPaintX = cellX;
						lastPaintY = cellY;
					}
				}
			}

			// マウスを離したらリセット
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
				lastPaintX = -1;
				lastPaintY = -1;
			}

			// ハイライト描画
			ImVec2 cellMin = ImVec2(canvasPos.x + cellX * gridSize,
				canvasPos.y + cellY * gridSize);
			ImVec2 cellMax = ImVec2(cellMin.x + gridSize,
				cellMin.y + gridSize);

			drawList->AddRectFilled(cellMin, cellMax, IM_COL32(200, 200, 50, 80));
		}

	}

	ImGui::End();


	// リサイズするとき
	if (isResize_)
	{
		// リサイズ
		ImGui::OpenPopup("Resize");

		bool isReturn_ = false;

		static int width = 0;
		static int height = 0;
		static char name[24] = "";

		// モーダルウィンドウ
		if (ImGui::BeginPopupModal("Resize", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::DragInt("Width", &width, 1, 0, 100);
			ImGui::DragInt("Height", &height, 1, 0, 100);
			ImGui::InputText("Name", name, IM_ARRAYSIZE(name));

			if (ImGui::Button("Return", ImVec2(120, 0)))
			{
				isReturn_ = true;
				isResize_ = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			// キャンセル
			if (ImGui::Button("Cancel", ImVec2(120, 0)))
			{
				isResize_ = false;
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}

		// 決定
		if (isReturn_)
		{
			stageData_.Resize(width, height, name);
		}
	}



	ImGui::Begin("Chip Selector");

	if (ImGui::Button("Air")) selectedTile_ = StageData::Tile::Air;
	ImGui::SameLine();
	if (ImGui::Button("Wall")) selectedTile_ = StageData::Tile::Wall;
	ImGui::SameLine();
	if (ImGui::Button("Player")) selectedTile_ = StageData::Tile::Player;

	ImGui::Text("Selected Chip: %d", static_cast<int32_t>(selectedTile_));

	ImGui::End();

	
#endif
}