#include "StageEditorUINavMesh.h"
#include "GrowthEngine.h"
#include "NavMesh/NavMesh.h"
#include "StageEditor/StageEditorNavMeshController/StageEditorNavMeshController.h"

/// @brief コンストラクタ
/// @param navMesh 
/// @param canExtrude 
/// @param canBridge 
/// @param isDirty 
void StageEditorUINavMesh::DrawUI(NavMesh* navMesh, bool canExtrude, bool canBridge, StageEditorNavMeshController* navMeshController, bool& isDirty)
{
	// ナビメッシュ編集モードのUIを描画
	ImGui::Text("--- ナビゲーションメッシュ ---");

	// ナビメッシュの島を追加するボタン
	if (ImGui::Button("ナビメッシュの島を追加"))
	{
		NavPolygon poly;
		poly.id = navMesh->GenerateNewPolygonId();

		// 新しい島の頂点を設定（XZ平面での正方形、Yは0固定）
		float size = 2.0f;
		poly.vertices[0] = Vector3(-size, 0.0f, -size);
		poly.vertices[1] = Vector3(-size, 0.0f, size);
		poly.vertices[2] = Vector3(size, 0.0f, size);
		poly.vertices[3] = Vector3(size, 0.0f, -size);

		// 完全に独立した新しい島なので、隣接IDはすべて無し (-1) に設定
		poly.neighborIds.fill(-1);

		// ナビメッシュに新しい島を追加
		navMesh->AddPolygon(poly);

		isDirty = true; // 変更があったフラグを立てる
	}

	ImGui::Separator();

	// 選択中のポリゴンを取得
	auto selectedItems = navMeshController->GetSelectedItems();

	if (!selectedItems.empty())
	{
		ImGui::Text("--- 選択中ポリゴン設定 ---");

		for (auto& item : selectedItems)
		{
			NavPolygon* selectedPoly = navMesh->GetMutablePolygon(item.polygonId);
			if (selectedPoly)
			{
				ImGui::Text("ポリゴンID: %d", selectedPoly->id);
				ImGui::Text("有効: %s", selectedPoly->isActive ? "true" : "false");
				ImGui::Text("隣接ID: [%d, %d, %d, %d]", selectedPoly->neighborIds[0], selectedPoly->neighborIds[1], selectedPoly->neighborIds[2], selectedPoly->neighborIds[3]);

				if (ImGui::InputInt("グループID", &selectedPoly->groupId))
				{
					isDirty = true; // 値が変更されたら保存用のフラグを立てる
				}
			}
		}
	}
}