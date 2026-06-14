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

	if (!ImGui::Begin("NavMesh インフォメーション", &isOpen_))
	{
		ImGui::End();
		return;
	}

	// 全体情報
	const auto& polygons = navMesh->GetPolygons();
	ImGui::Text("総ポリゴン数: %d", static_cast<int>(polygons.size()));
	ImGui::Separator();

	// 選択ステータスの表示と記憶の更新
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "--- 選択ステータス ---");

	// コントローラーが何か（ポリゴン・頂点・辺）を選択している場合
	if (controller && !controller->GetSelectedItems().empty())
	{
		// 選択された要素からポリゴンIDを取得して「記憶」を更新
		const auto& selected = controller->GetSelectedItems().front();
		lastSelectedPolygonId_ = selected.polygonId;

		// 現在のリアルタイムな選択情報を表示
		ImGui::Text("現在選択中のポリゴンID: %d", selected.polygonId);
		if (controller->GetSelectionMode() == StageEditorNavMeshController::SelectionMode::Vertex)
		{
			ImGui::Text("└ 選択中の頂点インデックス: %d", selected.itemIndex);
		}
		else if (controller->GetSelectionMode() == StageEditorNavMeshController::SelectionMode::Edge)
		{
			ImGui::Text("└ 選択中の辺インデックス: %d", selected.itemIndex);
		}
	}
	else
	{
		ImGui::Text("3Dビュー上の選択: なし");
	}

	ImGui::Dummy(ImVec2(0.0f, 2.0f));

	// 記憶されたポリゴンに対するアクション
	if (lastSelectedPolygonId_ != -1)
	{
		ImGui::Text("ターゲット対象のポリゴンID: %d", lastSelectedPolygonId_);

		if (ImGui::Button("このポリゴンを一覧で確認"))
		{
			targetPolygonId_ = lastSelectedPolygonId_;
		}

		ImGui::SameLine();
		if (ImGui::Button("入力をクリア"))
		{
			lastSelectedPolygonId_ = -1;
			targetPolygonId_ = -1;
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "(ポリゴンや辺などを選択するとここに操作ボタンが出ます)");
	}

	ImGui::Separator();

	// ポリゴンデータ一覧 (スクロール領域)
	ImGui::Text("--- ポリゴンデータ一覧 ---");
	ImGui::BeginChild("PolygonListChild", ImVec2(0, 0), true);

	for (const auto& poly : polygons)
	{
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

		// ボタンが押されてターゲット指定されているポリゴンは、自動で展開＆ハイライト
		if (poly.id == targetPolygonId_)
		{
			flags |= ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_DefaultOpen;
		}

		char nodeLabel[64];
		sprintf_s(nodeLabel, "Polygon ID: %d", poly.id);

		if (ImGui::TreeNodeEx(nodeLabel, flags))
		{
			// 頂点テーブル
			ImGui::Text("[頂点インデックス情報]");
			if (ImGui::BeginTable("VerticesTable", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				ImGui::TableSetupColumn("Index"); ImGui::TableSetupColumn("X"); ImGui::TableSetupColumn("Y"); ImGui::TableSetupColumn("Z");
				ImGui::TableHeadersRow();
				for (int i = 0; i < 4; ++i)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("%d", i);
					ImGui::TableSetColumnIndex(1); ImGui::Text("%.2f", poly.vertices[i].x);
					ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", poly.vertices[i].y);
					ImGui::TableSetColumnIndex(3); ImGui::Text("%.2f", poly.vertices[i].z);
				}
				ImGui::EndTable();
			}

			ImGui::Dummy(ImVec2(0.0f, 3.0f));

			// 辺・隣接テーブル
			ImGui::Text("[辺インデックス・隣接情報]");
			if (ImGui::BeginTable("NeighborsTable", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
			{
				ImGui::TableSetupColumn("辺 Index"); ImGui::TableSetupColumn("構成する頂点"); ImGui::TableSetupColumn("隣接ポリゴン ID");
				ImGui::TableHeadersRow();
				for (int i = 0; i < 4; ++i)
				{
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0); ImGui::Text("%d", i);
					ImGui::TableSetColumnIndex(1); ImGui::Text("%d -> %d", i, (i + 1) % 4);
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

	ImGui::End();
}