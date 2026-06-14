#include "StageEditorNavMeshController.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"
#include <set>

/// @brief 更新処理
/// @param placementList 
/// @param isDirty 
void StageEditorNavMeshController::Update(std::vector<PlacementData>& placementList, bool& isDirty)
{
	// 毎フレーム、マウスカーソルと床面との交点を取得
	Engine::Collision3D::Ray ray = RaycastFromMouse();
	Vector3 currentHitPoint = GetRayIntersectionWithPlane(ray, 0.0f); // 床の高さを0.0fとする

	// モード切り替え (1: 点, 2: 辺, 3: 面)
	if (engine_->GetKeyTrigger(DIK_1))
	{
		selectionMode_ = SelectionMode::Vertex;
		selectedItems_.clear();
	}
	if (engine_->GetKeyTrigger(DIK_2))
	{
		selectionMode_ = SelectionMode::Edge;
		selectedItems_.clear();
	}
	if (engine_->GetKeyTrigger(DIK_3))
	{
		selectionMode_ = SelectionMode::Polygon;
		selectedItems_.clear();
	}

	// 左クリックが押された瞬間（選択 ＆ ドラッグ開始）
	if (engine_->GetMouseButtonTrigger(MouseButton::Left))
	{
		// Shiftキーが押されているかどうかをチェック（複数選択のため）
		bool isShiftPressed = engine_->GetKeyPress(DIK_LSHIFT) || engine_->GetKeyPress(DIK_RSHIFT);

		SelectNavMeshItem();

		// Shiftキーが押されていない場合は、選択された辺だけを残して他の選択を解除する
		if (!selectedItems_.empty() && selectedItems_[0].polygonId != -1)
		{
			history_->SaveHistory(placementList);
			isDirty = true;

			isDraggingItem_ = true;
			previousHitPoint_ = currentHitPoint;
		}
	}
	// 左クリックが押し続けられている間（ドラッグ中）
	else if (engine_->GetMouseButtonPress(MouseButton::Left))
	{
		// 前回の交点から現在の交点への移動ベクトルを計算
		Vector3 moveDelta = currentHitPoint - previousHitPoint_;

		// 少しでもマウスが動いていれば辺を動かす
		if (std::abs(moveDelta.x) > 0.001f || std::abs(moveDelta.z) > 0.001f)
		{
			MoveSelectedItem(moveDelta);
		}

		// 次のフレームのために交点を更新
		previousHitPoint_ = currentHitPoint;
	}
	// 左クリックが離された瞬間（ドラッグ終了）
	else if (engine_->GetMouseButtonRelease(MouseButton::Left))
	{
		isDraggingItem_ = false;
	}

	// 押し出しとブリッジの操作（辺選択モードのときのみ）
	if (selectionMode_ == SelectionMode::Edge)
	{
		if (engine_->GetKeyTrigger(DIK_E)) ExtrudeSelectedEdge(placementList, isDirty);
		if (engine_->GetKeyTrigger(DIK_B)) BridgeSelectedEdges(placementList, isDirty);
	}

	// DeleteキーまたはBackspaceキーが押された瞬間
	if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace))
		DeleteSelectedNavMeshElements(placementList, isDirty);
}

/// @brief デバッグ用描画処理
void StageEditorNavMeshController::DrawDebug()
{
#ifdef _DEVELOPMENT

	// ナビメッシュが存在しない場合は描画しない
	if (!navMesh_) return;

	// ナビメッシュのデバッグ描画
	navMesh_->DrawDebug();
	DrawSelectedHighlight();


	// ブリッジ可能な状態なら、選択されている2つの辺を半透明の四角形で繋いでプレビュー表示する
	if (CanBridgeSelectedEdges())
	{
		// 選択されている2つのポリゴンを取得
		NavPolygon* poly1 = navMesh_->GetMutablePolygon(selectedItems_[0].polygonId);
		NavPolygon* poly2 = navMesh_->GetMutablePolygon(selectedItems_[1].polygonId);

		int eIdx1 = selectedItems_[0].itemIndex;
		int eIdx2 = selectedItems_[1].itemIndex;

		// 辺1の頂点
		Vector3 p1_v0 = poly1->vertices[eIdx1];
		Vector3 p1_v1 = poly1->vertices[(eIdx1 + 1) % 4];

		// 辺2の頂点
		Vector3 p2_v0 = poly2->vertices[eIdx2];
		Vector3 p2_v1 = poly2->vertices[(eIdx2 + 1) % 4];

		// プレビュー用の半透明な色（例：薄い水色）と、少し濃い境界線の色
		Vector4 previewFaceColor = { 0.0f, 1.0f, 1.0f, 0.3f };
		Vector4 previewLineColor = { 0.0f, 1.0f, 1.0f, 0.8f };

		// 2つの辺を繋いでできる四角形を、2つの三角形に分割して描画
		engine_->DrawDebugTriangle3D(p1_v1, p1_v0, p2_v1, previewFaceColor);
		engine_->DrawDebugTriangle3D(p1_v1, p2_v0, p2_v1, previewFaceColor);

		// 新しく作られる「橋渡し」の辺もうっすら描画
		engine_->DrawDebugLine3D(p1_v0, p2_v1, previewLineColor);
		engine_->DrawDebugLine3D(p1_v1, p2_v0, previewLineColor);
	}

#endif
}

/// @brief 辺を繋ぐ（ブリッジ）ことができるか
/// @return 
bool StageEditorNavMeshController::CanBridgeSelectedEdges() const
{
	if (selectionMode_ == SelectionMode::Edge && selectedItems_.size() == 2 && navMesh_ != nullptr)
	{
		NavPolygon* poly1 = navMesh_->GetMutablePolygon(selectedItems_[0].polygonId);
		NavPolygon* poly2 = navMesh_->GetMutablePolygon(selectedItems_[1].polygonId);

		if (poly1 && poly2 &&
			poly1->neighborIds[selectedItems_[0].itemIndex] == -1 &&
			poly2->neighborIds[selectedItems_[1].itemIndex] == -1 &&
			poly1->id != poly2->id)
		{
			Vector3 p1_v0 = poly1->vertices[selectedItems_[0].itemIndex];
			Vector3 p1_v1 = poly1->vertices[(selectedItems_[0].itemIndex + 1) % 4];
			Vector3 p2_v0 = poly2->vertices[selectedItems_[1].itemIndex];
			Vector3 p2_v1 = poly2->vertices[(selectedItems_[1].itemIndex + 1) % 4];

			auto IsSameVertex = [](const Vector3& a, const Vector3& b) {
				Vector3 diff = a - b;
				return (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z) < 0.01f;
				};

			// どの頂点も共有していない場合のみブリッジ可能とする
			if (!IsSameVertex(p1_v0, p2_v0) && !IsSameVertex(p1_v0, p2_v1) &&
				!IsSameVertex(p1_v1, p2_v0) && !IsSameVertex(p1_v1, p2_v1))
			{
				return true;
			}
		}
	}

	return false;
}

/// @brief マウスからレイキャストを飛ばして、NavMeshの辺を選択する処理
Engine::Collision3D::Ray StageEditorNavMeshController::RaycastFromMouse()
{
	// imguiのビューウィンドウ内のカーソルの位置を取得する
	Vector2 mouseScreenPos = engine_->GetMousePosition();

	// 正規化デバイス座標系
	float ndcX = (2.0f * mouseScreenPos.x) / static_cast<float>(1280.0f) - 1.0f;
	float ndcY = 1.0f - (2.0f * mouseScreenPos.y) / static_cast<float>(720.0f);

	// 同次クリップ空間
	Vector4 nearClip = { ndcX, ndcY, 0.0f, 1.0f };
	Vector4 farClip = { ndcX, ndcY, 1.0f, 1.0f };

	// 逆ビュープロジェクション行列
	Matrix4x4 invVP = engine_->GetCamera3DViewProjection().Inverse();

	Vector4 nearWorld = Transform(nearClip, invVP);
	Vector4 farWorld = Transform(farClip, invVP);

	// レイ
	Engine::Collision3D::Ray ray;
	ray.start = Vector3(nearWorld.x, nearWorld.y, nearWorld.z);

	ray.diff = Vector3(farWorld.x - nearWorld.x, farWorld.y - nearWorld.y, farWorld.z - nearWorld.z).Normalize();

	return ray;
}

/// @brief 選択されている辺を押し出す
void StageEditorNavMeshController::SelectNavMeshItem()
{
	if (!navMesh_) return;

	Engine::Collision3D::Ray ray = RaycastFromMouse();
	if (ray.diff.y >= 0.0f) return;

	float distanceToFloor = (0.0f - ray.start.y) / ray.diff.y;
	Vector3 hitPoint = ray.start + (ray.diff * distanceToFloor);

	int closestPolyId = -1;
	int closestItemIdx = -1;

	// === モードごとの当たり判定 ===
	if (selectionMode_ == SelectionMode::Vertex)
	{
		float minSqrDist = 1.0f;
		for (const auto& poly : navMesh_->GetPolygons()) 
		{
			for (int i = 0; i < 4; ++i) 
			{
				Vector3 v = poly.vertices[i];
				float sqrDist = (hitPoint.x - v.x) * (hitPoint.x - v.x) + (hitPoint.z - v.z) * (hitPoint.z - v.z); // XZ平面での距離
				if (sqrDist < minSqrDist)
				{
					minSqrDist = sqrDist;
					closestPolyId = poly.id;
					closestItemIdx = i;
				}
			}
		}
	}
	else if (selectionMode_ == SelectionMode::Edge)
	{
		float minSqrDist = 1.0f;
		for (const auto& poly : navMesh_->GetPolygons())
		{
			for (int i = 0; i < 4; ++i)
			{
				Vector3 v0 = poly.vertices[i];
				Vector3 v1 = poly.vertices[(i + 1) % 4];

				Vector3 ab = v1 - v0;
				Vector3 ap = hitPoint - v0;

				// 線分v0-v1上の点をパラメータtで表す（t=0ならv0、t=1ならv1）
				float t = (ap.x * ab.x + ap.y * ab.y + ap.z * ab.z) / (ab.x * ab.x + ab.y * ab.y + ab.z * ab.z);
				t = std::max(0.0f, std::min(1.0f, t));

				// 線分上の最近点を計算
				Vector3 closest(v0.x + t * ab.x, v0.y + t * ab.y, v0.z + t * ab.z);

				// 最近点とヒットポイントの距離の二乗を計算
				Vector3 diff = hitPoint - closest;
				float sqrDist = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

				if (sqrDist < minSqrDist) 
				{
					minSqrDist = sqrDist;
					closestPolyId = poly.id;
					closestItemIdx = i;
				}
			}
		}
	}
	else if (selectionMode_ == SelectionMode::Polygon)
	{
		// 面選択: NavPolygon の IsPointInside を使用して判定
		for (const auto& poly : navMesh_->GetPolygons()) 
		{
			if (poly.IsPointInside(hitPoint))
			{
				closestPolyId = poly.id;
				closestItemIdx = 0; // 面の場合はインデックス不要
				break;
			}
		}
	}

	// === 選択リストへの追加処理（複数選択対応） ===
	bool isMultiSelect = engine_->GetKeyPress(DIK_LSHIFT) || engine_->GetKeyPress(DIK_RSHIFT);

	if (closestPolyId != -1)
	{
		SelectedItem newItem{ closestPolyId, closestItemIdx };

		if (isMultiSelect) 
		{
			bool alreadySelected = false;
			auto it = selectedItems_.begin();

			for (; it != selectedItems_.end(); ++it)
			{
				if (it->polygonId == newItem.polygonId && it->itemIndex == newItem.itemIndex)
				{
					alreadySelected = true;
					break;
				}
			}

			if (!alreadySelected)
			{
				selectedItems_.push_back(newItem);
			}
			else 
			{
				selectedItems_.erase(it); // トグル選択解除
			}
		}
		else 
		{
			selectedItems_.clear();
			selectedItems_.push_back(newItem);
		}
	}
	else
	{
		if (!isMultiSelect) selectedItems_.clear();
	}
}


/// @brief 選択されている辺を押し出す
void StageEditorNavMeshController::ExtrudeSelectedEdge(std::vector<PlacementData>& placementList, bool& isDirty)
{
	if (selectedItems_.empty() || navMesh_ == nullptr) return;

	NavPolygon* poly = navMesh_->GetMutablePolygon(selectedItems_[0].polygonId);
	if (!poly) return;

	// すでに他のポリゴンと繋がっている辺からは押し出し不可にする
	if (poly->neighborIds[selectedItems_[0].itemIndex] != -1) return;

	// 押し出し前の状態を履歴に保存
	history_->SaveHistory(placementList);
	isDirty = true;

	int eIdx = selectedItems_[0].itemIndex;
	Vector3 v0 = poly->vertices[eIdx];
	Vector3 v1 = poly->vertices[(eIdx + 1) % 4];

	// 新しいポリゴンの作成
	NavPolygon newPoly;
	newPoly.id = navMesh_->GenerateNewPolygonId();
	newPoly.neighborIds = { -1, -1, -1, -1 }; // 初期化

	// 接合面の頂点は並び順を逆にセット（法線が裏返らないようにするため）
	newPoly.vertices[0] = v1;
	newPoly.vertices[1] = v0;

	// 元の辺のベクトルから法線方向（X, Z）を計算
	Vector3 edgeDir = v1 - v0;
	Vector3 normal(-edgeDir.z, 0, edgeDir.x);

	// ベクトルの正規化
	float length = std::sqrt(normal.x * normal.x + normal.z * normal.z);
	if (length > 0) { normal.x /= length; normal.z /= length; }

	// 押し出す距離 (例として2.0f)
	Vector3 offset(normal.x * 2.0f, 0.0f, normal.z * 2.0f);

	newPoly.vertices[2] = v0 + offset;
	newPoly.vertices[3] = v1 + offset;

	// 隣接情報(neighborIds)を相互に繋ぐ
	poly->neighborIds[eIdx] = newPoly.id;
	newPoly.neighborIds[0] = poly->id;

	// ナビメッシュに新しいポリゴンを追加
	navMesh_->AddPolygon(newPoly);

	// 連続して「E」を押せるように、選択状態を新しくできた先端の辺（インデックス2）に移動
	selectedItems_.clear();
	selectedItems_.push_back({ newPoly.id, 2 });
}

/// @brief レイとXZ平面（y=planeY）との交点を計算する関数
/// @param ray 
/// @param planeY 
/// @return 
Vector3 StageEditorNavMeshController::GetRayIntersectionWithPlane(const Engine::Collision3D::Ray& ray, float planeY)
{
	// レイが水平すぎて平面と交差しない場合は、始点をそのまま返す
	if (std::abs(ray.diff.y) < 0.0001f)
	{
		return ray.start;
	}

	// Y軸方向の距離から、交点までのパラメータ t を計算
	float t = (planeY - ray.start.y) / ray.diff.y;

	// 始点から t の距離だけ進んだ座標が交点
	return ray.start + (ray.diff * t);
}

/// @brief 選択された辺を移動する
/// @param moveDelta 
void StageEditorNavMeshController::MoveSelectedItem(const Vector3& moveDelta)
{
	if (selectedItems_.empty() || navMesh_ == nullptr) return;

	// --- 全ての選択アイテムを移動させる ---
	for (const auto& item : selectedItems_)
	{
		NavPolygon* targetPoly = navMesh_->GetMutablePolygon(item.polygonId);
		if (!targetPoly) continue;

		std::vector<Vector3> oldVertices;
		std::vector<Vector3> newVertices;

		if (selectionMode_ == SelectionMode::Vertex)
		{
			oldVertices.push_back(targetPoly->vertices[item.itemIndex]);
			newVertices.push_back(targetPoly->vertices[item.itemIndex] + moveDelta);
		}
		else if (selectionMode_ == SelectionMode::Edge)
		{
			oldVertices.push_back(targetPoly->vertices[item.itemIndex]);
			oldVertices.push_back(targetPoly->vertices[(item.itemIndex + 1) % 4]);
			newVertices.push_back(oldVertices[0] + moveDelta);
			newVertices.push_back(oldVertices[1] + moveDelta);
		}
		else if (selectionMode_ == SelectionMode::Polygon)
		{
			for (int i = 0; i < 4; ++i) {
				oldVertices.push_back(targetPoly->vertices[i]);
				newVertices.push_back(targetPoly->vertices[i] + moveDelta);
			}
		}

		// --- 共有頂点の同期更新 ---
		for (auto& poly : navMesh_->GetMutablePolygons())
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
}

/// @brief 選択された辺を繋ぐ（ブリッジ）する
void StageEditorNavMeshController::BridgeSelectedEdges(std::vector<PlacementData>& placementList, bool& isDirty)
{
	// 辺が2つ選択されていない場合は何もしない
	if (selectedItems_.size() != 2 || navMesh_ == nullptr) return;

	NavPolygon* poly1 = navMesh_->GetMutablePolygon(selectedItems_[0].polygonId);
	NavPolygon* poly2 = navMesh_->GetMutablePolygon(selectedItems_[1].polygonId);

	// 同じポリゴンの辺が選択されている場合は何もしない
	if (!poly1 || !poly2 || poly1->id == poly2->id) return;

	int eIdx1 = selectedItems_[0].itemIndex;
	int eIdx2 = selectedItems_[1].itemIndex;

	// 選択された辺がすでに他のポリゴンと繋がっている場合はキャンセル
	if (poly1->neighborIds[eIdx1] != -1 || poly2->neighborIds[eIdx2] != -1) return;

	// 履歴保存
	history_->SaveHistory(placementList);
	isDirty = true;

	// 2つの辺の頂点を取得
	Vector3 p1_v0 = poly1->vertices[eIdx1];
	Vector3 p1_v1 = poly1->vertices[(eIdx1 + 1) % 4];

	Vector3 p2_v0 = poly2->vertices[eIdx2];
	Vector3 p2_v1 = poly2->vertices[(eIdx2 + 1) % 4];

	// 選択された辺の頂点が同じポリゴンに属している場合はキャンセル（ブリッジ不可）
	auto IsSameVertex = [](const Vector3& a, const Vector3& b)
		{
			// 頂点が同じかどうかを判断するために、距離の2乗が小さいかで判定
			Vector3 diff = a - b;
			return (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z) < 0.01f;
		};

	// 共有頂点がある場合はブリッジ不可（複雑なトポロジーになるのを防ぐため）
	if (IsSameVertex(p1_v0, p2_v0) || IsSameVertex(p1_v0, p2_v1) || IsSameVertex(p1_v1, p2_v0) || IsSameVertex(p1_v1, p2_v1))
		return;

	// 新しいポリゴンの作成
	NavPolygon newPoly;
	newPoly.id = navMesh_->GenerateNewPolygonId();
	newPoly.neighborIds = { -1, -1, -1, -1 };

	// 頂点のねじれを防ぐため、距離の2乗を比較して結び方を決める
	// パターンA: p1_v0 と p2_v1、p1_v1 と p2_v0 を結ぶ
	Vector3 diffA1 = p1_v0 - p2_v1;
	Vector3 diffA2 = p1_v1 - p2_v0;
	float distSqA = (diffA1.x * diffA1.x + diffA1.y * diffA1.y + diffA1.z * diffA1.z) +
		(diffA2.x * diffA2.x + diffA2.y * diffA2.y + diffA2.z * diffA2.z);

	// パターンB: p1_v0 と p2_v0、p1_v1 と p2_v1 を結ぶ
	Vector3 diffB1 = p1_v0 - p2_v0;
	Vector3 diffB2 = p1_v1 - p2_v1;
	float distSqB = (diffB1.x * diffB1.x + diffB1.y * diffB1.y + diffB1.z * diffB1.z) +
		(diffB2.x * diffB2.x + diffB2.y * diffB2.y + diffB2.z * diffB2.z);

	// 接合面の頂点は、法線が裏返らないようにするため元の辺とは逆の順番でセットします
	newPoly.vertices[0] = p1_v1;
	newPoly.vertices[1] = p1_v0;

	if (distSqA < distSqB)
	{
		// パターンAの結び方が短い（ねじれない）場合
		newPoly.vertices[2] = p2_v1;
		newPoly.vertices[3] = p2_v0;
	}
	else
	{
		// パターンBの結び方が短い場合
		newPoly.vertices[2] = p2_v0;
		newPoly.vertices[3] = p2_v1;
	}

	// 隣接情報(neighborIds)を相互に繋ぐ
	// poly1 と newPoly
	poly1->neighborIds[eIdx1] = newPoly.id;
	newPoly.neighborIds[0] = poly1->id;

	// poly2 と newPoly (newPoly側の対向する辺はインデックス2)
	poly2->neighborIds[eIdx2] = newPoly.id;
	newPoly.neighborIds[2] = poly2->id;

	// ナビメッシュに追加
	navMesh_->AddPolygon(newPoly);

	// ブリッジ完了後は選択を解除（または新しいポリゴンを選択状態にする）
	selectedItems_.clear();
}

/// @brief 選択された辺を削除する
void StageEditorNavMeshController::DeleteSelectedNavMeshElements(std::vector<PlacementData>& placementList, bool& isDirty)
{
	if (selectedItems_.empty()) return;

	// 削除対象のポリゴンIDを格納するセット（重複を防ぐため）
	std::set<int> polygonsToRemove;

	for (const auto& item : selectedItems_)
	{
		NavPolygon* poly = navMesh_->GetMutablePolygon(item.polygonId);
		if (!poly) continue;

		switch (selectionMode_)
		{
		case SelectionMode::Polygon:
			// 【面削除】選択されたポリゴンそのものを削除対象にする
			polygonsToRemove.insert(item.polygonId);
			break;

		case SelectionMode::Edge:
			// 【辺削除】選択されたポリゴン自身と、その辺を共有して隣接しているポリゴンを削除対象にする
			polygonsToRemove.insert(item.polygonId);
			if (poly->neighborIds[item.itemIndex] != -1)
			{
				polygonsToRemove.insert(poly->neighborIds[item.itemIndex]);
			}
			break;

		case SelectionMode::Vertex:
		{
			// 【点削除】選択された頂点の座標を取得
			Vector3 targetVertex = poly->vertices[item.itemIndex];

			// 全ポリゴンを走査し、同じ座標の頂点を持つポリゴンをすべて抽出
			for (const auto& p : navMesh_->GetPolygons())
			{
				for (int i = 0; i < 4; ++i)
				{
					// 浮動小数点の誤差を考慮して座標がほぼ一致するか判定
					if ((p.vertices[i] - targetVertex).LengthSq() < 0.0001f)
					{
						polygonsToRemove.insert(p.id);
						break; // このポリゴンは確定なので次のポリゴンへ
					}
				}
			}
		}
		break;
		}
	}

	// 削除対象がなければ終了
	if (polygonsToRemove.empty()) return;

	// 変更前の状態を履歴に保存
	history_->SaveHistory(placementList);

	// ナビメッシュから削除対象のポリゴンをすべて削除する
	auto& polygons = navMesh_->GetMutablePolygons();
	polygons.erase(
		std::remove_if(polygons.begin(), polygons.end(),
			[&polygonsToRemove](const NavPolygon& p) {
				return polygonsToRemove.count(p.id) > 0;
			}),
		polygons.end()
	);

	// 削除されたポリゴンと隣接していたポリゴンの neighborIds を -1 にリセットする
	for (auto& p : polygons)
	{
		for (int i = 0; i < 4; ++i)
		{
			if (polygonsToRemove.count(p.neighborIds[i]) > 0)
			{
				p.neighborIds[i] = -1;
			}
		}
	}

	// 選択状態を安全にリセット
	selectedItems_.clear();
}

/// @brief 選択された辺をハイライト表示する
void StageEditorNavMeshController::DrawSelectedHighlight()
{
	if (!navMesh_ || selectedItems_.empty()) return;

	for (const auto& item : selectedItems_)
	{
		NavPolygon* poly = navMesh_->GetMutablePolygon(item.polygonId);
		if (!poly) continue;

		if (selectionMode_ == SelectionMode::Vertex)
		{
			Vector3 v = poly->vertices[item.itemIndex];
			Vector3 renderV = Vector3(v.x, v.y + 0.05f, v.z);
			// 点を強調表示（Engine側に球やBoxを描画する機能があればそれを使用。例として短い線を描画）
			engine_->DrawDebugLine3D(renderV, renderV + Vector3(0, 0.5f, 0), Vector4(1.0f, 1.0f, 0.0f, 1.0f));
		}
		else if (selectionMode_ == SelectionMode::Edge)
		{
			Vector3 v0 = poly->vertices[item.itemIndex];
			Vector3 v1 = poly->vertices[(item.itemIndex + 1) % 4];
			Vector3 renderV0 = Vector3(v0.x, v0.y + 0.01f, v0.z);
			Vector3 renderV1 = Vector3(v1.x, v1.y + 0.01f, v1.z);
			engine_->DrawDebugLine3D(renderV0, renderV1, Vector4(1.0f, 1.0f, 0.0f, 1.0f));
		}
		else if (selectionMode_ == SelectionMode::Polygon)
		{
			// 面の強調表示（半透明の三角形などで塗りつぶす）
			Vector3 v0 = poly->vertices[0]; Vector3 v1 = poly->vertices[1];
			Vector3 v2 = poly->vertices[2]; Vector3 v3 = poly->vertices[3];
			v0.y += 0.01f; v1.y += 0.01f; v2.y += 0.01f; v3.y += 0.01f;

			engine_->DrawDebugTriangle3D(v0, v1, v2, Vector4(1.0f, 1.0f, 0.0f, 0.5f));
			engine_->DrawDebugTriangle3D(v0, v2, v3, Vector4(1.0f, 1.0f, 0.0f, 0.5f));
		}
	}
}
