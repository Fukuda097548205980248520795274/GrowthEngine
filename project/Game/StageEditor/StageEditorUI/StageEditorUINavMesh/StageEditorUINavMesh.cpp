#include "StageEditorUINavMesh.h"
#include "GrowthEngine.h"
#include "NavMesh/NavMesh.h"

/// @brief コンストラクタ
/// @param navMesh 
/// @param canExtrude 
/// @param canBridge 
/// @param isDirty 
void StageEditorUINavMesh::DrawUI(NavMesh* navMesh, bool canExtrude, bool canBridge, bool& isDirty)
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
}