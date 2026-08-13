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
void StageEditorGuizmo::UpdateObject(std::vector<PlacementData>& placementList, int selectedIndex, bool& isDirty, StageEditorHistory* history, bool isPlaying)
{
	// ゲームが再生中の場合はギズモの操作を無効化する
	if (isPlaying || !engine_->IsCursorWindowHover())return;

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

		// ギズモの操作モードを切り替えるためのキー入力をチェック
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
			constexpr float kDeg2Rad = std::numbers::pi_v<float> / 180.0f;

			switch (currentOperation_)
			{
			case ImGuizmo::TRANSLATE:
				// 移動成分抽出
				data.position = Vector3(translation[0], translation[1], translation[2]);
				break;
			case ImGuizmo::ROTATE:
				// 回転成分抽出
				data.rotate_.x = rotation[0] * kDeg2Rad;
				data.rotate_.y = rotation[1] * kDeg2Rad;
				data.rotate_.z = rotation[2] * kDeg2Rad;
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

		// ギズモを使ってオブジェクトを操作中かどうかのフラグを更新
		useObjectSnap_ = ImGuizmo::IsUsing();
	}
}


/// @brief ナビメッシュの変形を更新する
/// @param navMesh 
/// @param navMeshController 
/// @param isDirty 
/// @param history 
void StageEditorGuizmo::UpdateNavMesh(NavMesh* navMesh, StageEditorNavMeshController* navMeshController, bool& isDirty, StageEditorHistory* history, bool isPlaying)
{
	// ゲームが再生中の場合はナビメッシュの編集を行わない
	if (isPlaying)return;

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

	// 選択されているすべての要素から「中心座標（重心）」を計算する
	Vector3 center = { 0.0f, 0.0f, 0.0f };
	int vertexCount = 0;

	for (const auto& selectedItem : selectedItems)
	{
		NavPolygon* poly = navMesh->GetMutablePolygon(selectedItem.polygonId);
		if (!poly) continue;

		if (selectionMode == StageEditorNavMeshController::SelectionMode::Vertex)
		{
			center = center + poly->vertices[selectedItem.itemIndex];
			vertexCount++;
		} 
		else if (selectionMode == StageEditorNavMeshController::SelectionMode::Edge)
		{
			center = center + poly->vertices[selectedItem.itemIndex];
			center = center + poly->vertices[(selectedItem.itemIndex + 1) % 4];
			vertexCount += 2;
		} 
		else if (selectionMode == StageEditorNavMeshController::SelectionMode::Polygon)
		{
			for (int i = 0; i < 4; ++i)
			{
				center = center + poly->vertices[i];
				vertexCount++;
			}
		}
	}

	if (vertexCount > 0)
	{
		center = center / static_cast<float>(vertexCount);
	}

	// ワールド行列を選択要素の中心位置に設定
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

		// 変更前の頂点と変更後の頂点をペアで保持するための構造体リスト
		struct VertexMove {
			Vector3 oldPos;
			Vector3 newPos;
		};
		std::vector<VertexMove> vertexMoves;

		// 各要素の頂点に対して移動量を加算し、リストに記録する
		for (const auto& selectedItem : selectedItems)
		{
			NavPolygon* poly = navMesh->GetMutablePolygon(selectedItem.polygonId);
			if (!poly) continue;

			if (selectionMode == StageEditorNavMeshController::SelectionMode::Vertex)
			{
				Vector3 oldPos = poly->vertices[selectedItem.itemIndex];
				vertexMoves.push_back({ oldPos, oldPos + move });
			}
			else if (selectionMode == StageEditorNavMeshController::SelectionMode::Edge)
			{
				Vector3 oldPos0 = poly->vertices[selectedItem.itemIndex];
				Vector3 oldPos1 = poly->vertices[(selectedItem.itemIndex + 1) % 4];
				vertexMoves.push_back({ oldPos0, oldPos0 + move });
				vertexMoves.push_back({ oldPos1, oldPos1 + move });
			} 
			else if (selectionMode == StageEditorNavMeshController::SelectionMode::Polygon)
			{
				for (int i = 0; i < 4; ++i)
				{
					Vector3 oldPos = poly->vertices[i];
					vertexMoves.push_back({ oldPos, oldPos + move });
				}
			}
		}

		// 共有頂点の同期更新
		for (auto& poly : navMesh->GetMutablePolygons())
		{
			for (int i = 0; i < 4; ++i)
			{
				for (const auto& vm : vertexMoves)
				{
					Vector3 diff = poly.vertices[i] - vm.oldPos;

					// 浮動小数点の誤差を考慮して同じ頂点か判定
					if ((diff.x * diff.x + diff.y * diff.y + diff.z * diff.z) < 0.01f)
					{
						poly.vertices[i] = vm.newPos;
						break; // 一度更新したら次の頂点へ（重複更新を防ぐ）
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

	// ギズモを操作している間は、スナップ機能を有効にする（オプション）
	useNavMeshSnap_ = ImGuizmo::IsUsing();
}