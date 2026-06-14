#include "StageEditorUINavMeshInfo.h"
#include "GrowthEngine.h"
#include "NavMesh/NavMesh.h"
#include "StageEditor/StageEditorNavMeshController/StageEditorNavMeshController.h"

/// @brief ウィンドウの描画処理
/// @param navMesh 
/// @param controller 
void StageEditorUINavMeshInfo::DrawWindow(NavMesh* navMesh, const StageEditorNavMeshController* controller)
{
	if (!navMesh) return;

	// メインメニューや別の場所から開閉できるように、ImGuiのBeginで制御
	if (!ImGui::Begin("NavMesh インフォメーション", &isOpen_))
	{
		ImGui::End();
		return;
	}


	// 全体情報
	const auto& polygons = navMesh->GetPolygons();
	ImGui::Text("総ポリゴン数: %d", static_cast<int>(polygons.size()));
	ImGui::Separator();


	// コントローラーの現在の選択状態を表示
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "--- 現在の選択状態 ---");
	if (controller && !controller->GetSelectedItems().empty())
	{
		// 最初の選択要素を代表として表示
		const auto& selected = controller->GetSelectedItems().front();
		ImGui::Text("選択中のポリゴンID: %d", selected.polygonId);

		if (controller->GetSelectionMode() == StageEditorNavMeshController::SelectionMode::Vertex)
		{
			ImGui::Text("選択中の頂点インデックス: %d", selected.itemIndex);
		}
		else if (controller->GetSelectionMode() == StageEditorNavMeshController::SelectionMode::Edge)
		{
			ImGui::Text("選択中の辺インデックス: %d", selected.itemIndex);
		}

		// ボタンを押すと、そのポリゴンの詳細へ自動スクロール/選択させるためのID保持
		if (ImGui::Button("このポリゴンを一覧で確認"))
		{
			targetPolygonId_ = selected.polygonId;
		}
	}
	else
	{
		ImGui::Text("何も選択されていません");
	}
	ImGui::Separator();


	// ポリゴンインデックス詳細一覧 (テーブル形式)
	ImGui::Text("--- ポリゴンデータ一覧 ---");

	// 子ウィンドウにしてスクロール可能にする
	ImGui::BeginChild("PolygonListChild", ImVec2(0, 0), true);

	for (const auto& poly : polygons)
	{
		// ツリーノード（折りたたみ）ヘッダーを作成。ターゲット指定されている場合は開く
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
		if (poly.id == targetPolygonId_)
		{
			flags |= ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_DefaultOpen;
		}

		char nodeLabel[64];
		sprintf_s(nodeLabel, "Polygon ID: %d", poly.id);

		if (ImGui::TreeNodeEx(nodeLabel, flags))
		{
			// 頂点インデックスと座標のテーブル
			ImGui::Text("[頂点インデックス情報]");
			if (ImGui::BeginTable("VerticesTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				ImGui::TableSetupColumn("Index");
				ImGui::TableSetupColumn("X");
				ImGui::TableSetupColumn("Y");
				ImGui::TableSetupColumn("Z");
				ImGui::TableHeadersRow();

				for (int i = 0; i < 4; ++i)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%d", i);
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%.2f", poly.vertices[i].x);
					ImGui::TableSetColumnIndex(2);
					ImGui::Text("%.2f", poly.vertices[i].y);
					ImGui::TableSetColumnIndex(3);
					ImGui::Text("%.2f", poly.vertices[i].z);
				}
				ImGui::EndTable();
			}

			ImGui::Dummy(ImVec2(0.0f, 5.0f)); // 少し隙間を空ける

			// 辺インデックスと隣接関係の情報
			ImGui::Text("[辺インデックス・隣接情報]");
			if (ImGui::BeginTable("NeighborsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				ImGui::TableSetupColumn("辺 Index");
				ImGui::TableSetupColumn("構成する頂点");
				ImGui::TableSetupColumn("隣接ポリゴン ID");
				ImGui::TableHeadersRow();

				for (int i = 0; i < 4; ++i)
				{
					ImGui::TableNextRow();

					// 辺インデックス
					ImGui::TableSetColumnIndex(0);
					ImGui::Text("%d", i);

					// 構成する頂点 (i -> (i+1)%4)
					ImGui::TableSetColumnIndex(1);
					ImGui::Text("%d -> %d", i, (i + 1) % 4);

					// 隣接ID (-1の場合は分かりやすく警告色にする)
					ImGui::TableSetColumnIndex(2);
					if (poly.neighborIds[i] == -1)
					{
						ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "None (オープンエッジ)");
					}
					else
					{
						ImGui::Text("ID: %d", poly.neighborIds[i]);
					}
				}
				ImGui::EndTable();
			}

			ImGui::Separator();
			ImGui::TreePop();
		}
	}
	ImGui::EndChild();

	// ターゲット確認が終わったらリセット
	if (ImGui::IsMouseClicked(0) && !ImGui::IsAnyItemHovered())
	{
		targetPolygonId_ = -1;
	}

	ImGui::End();
}