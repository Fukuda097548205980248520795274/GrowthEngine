#include "StageEditorGuizmo.h"
#include <numbers>

#include "StageEditor/StageEditorHistory/StageEditorHistory.h"
#include "StageEditor/StageEditorNavMeshController/StageEditorNavMeshController.h"
#include "NavMesh/NavMesh.h"

#include "Entity/Character/NPC/NPC.h"
#include "Entity/Character/Player/Player.h"
#include "Entity/Weapon/Weapon.h"

#include "StageObject/Floor/Floor.h"
#include "StageObject/Wall/Wall.h"
#include "StageObject/StaticEventTrigger/StaticEventTrigger.h"


/// @brief オブジェクトの変形を更新する
/// @param placementList 
/// @param selectedIndex 
/// @param isDirty 
/// @param history 
void StageEditorGuizmo::UpdateObject(std::vector<PlacementData>& placementList, int selectedIndex, bool& isDirty, StageEditorHistory* history)
{
	// 選択されているオブジェクトがあれば、そのオブジェクトをギズモで操作できるようにする
	if (selectedIndex >= 0 && selectedIndex < placementList.size())
	{
		auto& data = placementList[selectedIndex];

		// カメラのビュー行列とプロジェクション行列を取得
		Matrix4x4 viewMatrix = engine_->GetCamera3DView();
		Matrix4x4 projectionMatrix = engine_->GetCamera3DProjection();

		// 現在のSRTからワールド行列を一度だけ生成する
		Quaternion rotateQ =
			ToQuaternion(data.rotate_.z, Vector3(0.0f, 0.0, 1.0f)).Normalize() *
			ToQuaternion(data.rotate_.y, Vector3(0.0f, 1.0, 0.0f)).Normalize() *
			ToQuaternion(data.rotate_.x, Vector3(1.0f, 0.0, 0.0f)).Normalize();

		// ワールド行列
		Matrix4x4 worldMatrix =
			Make3DScaleMatrix4x4(data.scale) *
			Make3DRotateMatrix4x4(rotateQ) *
			Make3DTranslateMatrix4x4(data.position);

		// キーボード入力などで操作モードを切り替える例
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsKeyPressed(ImGuiKey_T) && !io.KeyCtrl) currentOperation_ = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(ImGuiKey_R) && !io.KeyCtrl) currentOperation_ = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(ImGuiKey_S) && !io.KeyCtrl) currentOperation_ = ImGuizmo::SCALE;

		// ギズモの描画と操作
		ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projectionMatrix.m[0][0], currentOperation_, currentMode_, &worldMatrix.m[0][0]);

		// ギズモを使ってオブジェクトを操作中かどうかをチェック
		if (ImGuizmo::IsUsing() && useObjectSnap_)
		{
			float translation[3];
			float rotation[3];
			float scale[3];

			// ワールド行列からSRT成分を抽出
			ImGuizmo::DecomposeMatrixToComponents(&worldMatrix.m[0][0], translation, rotation, scale);

			// 回転はImGuizmoが度単位で返すので、ラジアンに変換するための定数
			constexpr float DEG2RAD = std::numbers::pi_v<float> / 180.0f;

			switch (currentOperation_)
			{
			case ImGuizmo::TRANSLATE:
				// 移動成分抽出
				data.position = Vector3(translation[0], translation[1], translation[2]);
				break;
			case ImGuizmo::ROTATE:
				// 回転成分抽出
				data.rotate_.x = rotation[0] * DEG2RAD;
				data.rotate_.y = rotation[1] * DEG2RAD;
				data.rotate_.z = rotation[2] * DEG2RAD;
				break;
			case ImGuizmo::SCALE:
				// 拡縮成分抽出
				data.scale = Vector3(scale[0], scale[1], scale[2]);
				break;
			}

			// ギズモで操作した結果をゲーム内の実体に反映させる
			if (data.category == EditCategory::Character || data.category == EditCategory::Weapon)
			{
				auto entityPtr = static_cast<Entity*>(data.instancePtr);
				entityPtr->SetPosition(data.position);
				entityPtr->SetRotation(data.rotate_);
				entityPtr->SetScale(data.scale);
			}
			else if (data.category == EditCategory::Object)
			{
				auto stageObjectPtr = static_cast<StageObject*>(data.instancePtr);
				stageObjectPtr->SetPosition(data.position);
				stageObjectPtr->SetRotation(data.rotate_);
				stageObjectPtr->SetScale(data.scale);
			}
		}
		else
		{
			// ギズモを操作していないときは、ゲーム内の実体の位置を配置データに反映させる
			if (data.category == EditCategory::Character || data.category == EditCategory::Weapon)
			{
				auto entityPtr = static_cast<Entity*>(data.instancePtr);
				data.position = entityPtr->GetWorldTransform()->translate_;
				data.rotate_ = entityPtr->GetWorldTransform()->rotate_;
				data.scale = entityPtr->GetWorldTransform()->scale_;
			}
			else if (data.category == EditCategory::Object)
			{
				auto stageObjectPtr = static_cast<StageObject*>(data.instancePtr);
				data.position = stageObjectPtr->GetWorldTransform()->translate_;
				data.rotate_ = stageObjectPtr->GetWorldTransform()->rotate_;
				data.scale = stageObjectPtr->GetWorldTransform()->scale_;
			}
		}

		// Guizmoを触った瞬間に配置データの変更を確定させる（スナップ機能を使用していない場合）
		if (ImGuizmo::IsUsing() && !useObjectSnap_)
		{
			// 配置データを更新
			history->SaveHistory(placementList);
			isDirty = true;
		}

		// ギズモを操作している間は、スナップ機能を有効にする例（オプション）
		useObjectSnap_ = ImGuizmo::IsUsing();
	}
}


/// @brief ナビメッシュの変形を更新する
/// @param navMesh 
/// @param navMeshController 
/// @param isDirty 
/// @param history 
void StageEditorGuizmo::UpdateNavMesh(NavMesh* navMesh, StageEditorNavMeshController* navMeshController, bool& isDirty, StageEditorHistory* history)
{
	// ナビメッシュやコントローラーが存在しない場合は何もしない
	if (navMesh == nullptr || navMeshController == nullptr) return;

	// ナビメッシュの選択されている要素を取得
	auto selectedItems = navMeshController->GetSelectedItems();

	// 選択されている要素がない場合は何もしない
	if (selectedItems.empty()) return;

	// 選択モードに応じて、頂点、辺、または面全体を移動する
	StageEditorNavMeshController::SelectionMode selectionMode = navMeshController->GetSelectionMode();

	// カメラのビュー行列とプロジェクション行列を取得
	Matrix4x4 viewMatrix = engine_->GetCamera3DView();
	Matrix4x4 projectionMatrix = engine_->GetCamera3DProjection();

	for(auto& selectedItem : selectedItems)
	{
		// 選択されているポリゴンを取得
		NavPolygon* poly = navMesh->GetMutablePolygon(selectedItem.polygonId);
		if (!poly) continue;

		// 選択されている要素の頂点を取得
		std::vector<Vector3> oldVertices;
		std::vector<Vector3> newVertices;

		if (selectionMode == StageEditorNavMeshController::SelectionMode::Vertex)
		{
			// 選択されている頂点を取得
			Vector3 position = poly->vertices[selectedItem.itemIndex];
			oldVertices.push_back(poly->vertices[selectedItem.itemIndex]);

			// ワールド行列を頂点の位置に設定
			Matrix4x4 worldMatrix = Make3DTranslateMatrix4x4(position);

			// ギズモの描画と操作
			ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projectionMatrix.m[0][0], ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, &worldMatrix.m[0][0]);

			// ギズモを使ってオブジェクトを操作中かどうかをチェック
			if (ImGuizmo::IsUsing() && useNavMeshSnap_)
			{
				float translation[3];
				float rotation[3];
				float scale[3];

				// ワールド行列からSRT成分を抽出
				ImGuizmo::DecomposeMatrixToComponents(&worldMatrix.m[0][0], translation, rotation, scale);
				Vector3 movePosition = Vector3(translation[0], translation[1], translation[2]);

				// 移動成分抽出
				newVertices.push_back(movePosition);
			}
			else
			{
				newVertices.push_back(poly->vertices[selectedItem.itemIndex]);
			}
		}
		else if (selectionMode == StageEditorNavMeshController::SelectionMode::Edge)
		{
			// 選択されている辺の両端の頂点を取得
			Vector3 position = (poly->vertices[(selectedItem.itemIndex + 1) % 4] - poly->vertices[selectedItem.itemIndex]) / 2.0f + poly->vertices[selectedItem.itemIndex];
			oldVertices.push_back(poly->vertices[selectedItem.itemIndex]);
			oldVertices.push_back(poly->vertices[(selectedItem.itemIndex + 1) % 4]);

			// ワールド行列を頂点の位置に設定
			Matrix4x4 worldMatrix = Make3DTranslateMatrix4x4(position);

			// ギズモの描画と操作
			ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projectionMatrix.m[0][0], ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, &worldMatrix.m[0][0]);

			// ギズモを使ってオブジェクトを操作中かどうかをチェック
			if (ImGuizmo::IsUsing() && useNavMeshSnap_)
			{
				float translation[3];
				float rotation[3];
				float scale[3];

				// ワールド行列からSRT成分を抽出
				ImGuizmo::DecomposeMatrixToComponents(&worldMatrix.m[0][0], translation, rotation, scale);
				Vector3 move = Vector3(translation[0], translation[1], translation[2]) - position;

				// 移動成分抽出
				newVertices.push_back(oldVertices[0] + move);
				newVertices.push_back(oldVertices[1] + move);
			}
			else
			{
				newVertices.push_back(oldVertices[0]);
				newVertices.push_back(oldVertices[1]);
			}
		}
		else if (selectionMode == StageEditorNavMeshController::SelectionMode::Polygon)
		{
			// 面の4頂点すべてを保存
			for (int i = 0; i < 4; ++i)
			{
				oldVertices.push_back(poly->vertices[i]);
			}

			// 面の中心座標（4頂点の平均）を計算
			Vector3 center = (poly->vertices[0] + poly->vertices[1] + poly->vertices[2] + poly->vertices[3]) / 4.0f;

			// ワールド行列を面の中心位置に設定
			Matrix4x4 worldMatrix = Make3DTranslateMatrix4x4(center);

			// ギズモの描画と操作
			ImGuizmo::Manipulate(&viewMatrix.m[0][0], &projectionMatrix.m[0][0], ImGuizmo::TRANSLATE, ImGuizmo::LOCAL, &worldMatrix.m[0][0]);

			// ギズモを使ってオブジェクトを操作中かどうかをチェック
			if (ImGuizmo::IsUsing() && useNavMeshSnap_)
			{
				float translation[3];
				float rotation[3];
				float scale[3];

				// ワールド行列からSRT成分を抽出
				ImGuizmo::DecomposeMatrixToComponents(&worldMatrix.m[0][0], translation, rotation, scale);

				// 移動量（現在のギズモ座標 - 元の中心座標）を計算
				Vector3 move = Vector3(translation[0], translation[1], translation[2]) - center;

				// 4つの頂点すべてに移動量を加算
				for (int i = 0; i < 4; ++i)
				{
					newVertices.push_back(oldVertices[i] + move);
				}
			}
			else
			{
				// 操作していない場合は元の頂点座標を維持
				for (int i = 0; i < 4; ++i)
				{
					newVertices.push_back(oldVertices[i]);
				}
			}
		}


		// --- 共有頂点の同期更新 ---
		for (auto& poly : navMesh->GetMutablePolygons())
		{
			for (int i = 0; i < 4; ++i)
			{
				for (size_t vIdx = 0; vIdx < oldVertices.size(); ++vIdx)
				{
					Vector3 diff = poly.vertices[i] - oldVertices[vIdx];
					if ((diff.x * diff.x + diff.y * diff.y + diff.z * diff.z) < 0.01f)
					{
						poly.vertices[i] = newVertices[vIdx];
					}
				}
			}
		}
	}

	// Guizmoを触った瞬間に配置データの変更を確定させる（スナップ機能を使用していない場合）
	if (ImGuizmo::IsUsing() && !useNavMeshSnap_)
	{
		history->SaveHistory();
		isDirty = true;
	}

	// ギズモを操作している間は、スナップ機能を有効にする例（オプション）
	useNavMeshSnap_ = ImGuizmo::IsUsing();
}