#include "StageEditorNavMeshController.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"
#include <set>

/// @brief 更新処理
/// @param placementList 
/// @param isDirty 
void StageEditorNavMeshController::Update(std::vector<PlacementData>& placementList, bool& isDirty)
{
	// 選択されているポリゴンの高さを取得（押し出し時の基準高さとして使用）
	float baseHeight = 0.0f;
	if (!selectedItems_.empty())
	{
		NavPolygon* poly = navMesh_->GetMutablePolygon(selectedItems_[0].polygonId);
		if (poly)baseHeight = poly->vertices[0].y;
	}

	// 毎フレーム、マウスカーソルと床面との交点を取得
	Engine::Collision3D::Ray ray = RaycastFromMouse();
	Vector3 currentHitPoint = GetRayIntersectionWithPlane(ray, baseHeight);

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
	if (engine_->IsCursorWindowHover() && engine_->GetMouseButtonTrigger(MouseButton::Left) && !ImGuizmo::IsOver())
	{
		// Shiftキーが押されているかどうかをチェック（複数選択のため）
		bool isShiftPressed = engine_->GetKeyPress(DIK_LSHIFT) || engine_->GetKeyPress(DIK_RSHIFT);

		SelectNavMeshItem();

		// Shiftキーが押されていない場合は、選択された辺だけを残して他の選択を解除する
		if (!selectedItems_.empty() && selectedItems_[0].polygonId != -1)
		{
			history_->SaveHistory(placementList);
			isDirty = true;
		}
	}

	// 押し出しとブリッジの操作（辺選択モードのときのみ）
	if (selectionMode_ == SelectionMode::Edge)
	{
		if (engine_->GetKeyTrigger(DIK_E)) ExtrudeSelectedEdge(placementList, isDirty);
		if (engine_->GetKeyTrigger(DIK_B)) BridgeSelectedEdges(placementList, isDirty);
	}

	// 選択された面を有効・無効化する（面選択モードのときのみ）
	if (selectionMode_ == SelectionMode::Polygon)
	{
		if (engine_->GetKeyTrigger(DIK_A)) ActivePolygon(placementList, isDirty);
	}

	// DeleteキーまたはBackspaceキーが押された瞬間
	if (ImGui::IsKeyPressed(ImGuiKey_Delete) || ImGui::IsKeyPressed(ImGuiKey_Backspace))
		DeleteSelectedNavMeshElements(placementList, isDirty);
}

/// @brief デバッグ用描画処理
void StageEditorNavMeshController::DrawDebug()
{
#ifdef DEVELOPMENT

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

		// プレビュー用の半透明な色と、少し濃い境界線の色
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

/// @brief マウスカーソルからのレイを計算する関数
/// @return 
Engine::Collision3D::Ray StageEditorNavMeshController::RaycastFromMouse()
{
	// imguiのビューウィンドウ内のカーソルの位置を取得する
	Vector2 mouseScreenPos = engine_->GetMousePosition();

	// 正規化デバイス座標系
	float ndcX = (2.0f * mouseScreenPos.x) / static_cast<float>(GrowthEngine::GetInstance()->GetScreenWidth()) - 1.0f;
	float ndcY = 1.0f - (2.0f * mouseScreenPos.y) / static_cast<float>(GrowthEngine::GetInstance()->GetScreenHeight());

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

/// @brief ナビゲーションメッシュの選択
void StageEditorNavMeshController::SelectNavMeshItem()
{
	if (!navMesh_) return;

	Engine::Collision3D::Ray ray = RaycastFromMouse();

	int closestPolyId = -1;
	int closestItemIdx = -1;

	// 点・辺の選択判定用（3D距離の2乗）
	float minSqrDist = 1.0f;
	// 面選択用（カメラから一番手前にある面を選ぶための深度）
	float closestHitT = FLT_MAX;

	for (const auto& poly : navMesh_->GetPolygons())
	{
		// ポリゴンの近似法線を計算 (頂点0, 1, 2を使用)
		Vector3 v0 = poly.vertices[0];
		Vector3 v1 = poly.vertices[1];
		Vector3 v2 = poly.vertices[2];

		Vector3 edge1 = v1 - v0;
		Vector3 edge2 = v2 - v0;

		// 外積で法線ベクトルを計算
		Vector3 normal(
			edge1.y * edge2.z - edge1.z * edge2.y,
			edge1.z * edge2.x - edge1.x * edge2.z,
			edge1.x * edge2.y - edge1.y * edge2.x
		);

		// 法線の正規化
		float nLen = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		if (nLen > 0.0001f) {
			normal.x /= nLen; normal.y /= nLen; normal.z /= nLen;
		}

		// レイと「このポリゴンの平面」の交差距離 t を計算
		float dotND = normal.x * ray.diff.x + normal.y * ray.diff.y + normal.z * ray.diff.z;

		// レイと平面がほぼ平行な場合はスキップ
		if (std::abs(dotND) < 0.0001f) continue;

		Vector3 toV0 = v0 - ray.start;
		float t = (normal.x * toV0.x + normal.y * toV0.y + normal.z * toV0.z) / dotND;

		// 視点より後ろにある場合はスキップ
		if (t < 0.0f) continue;

		// このポリゴンの平面上での交点 (3D座標)
		Vector3 hitPoint = ray.start + (ray.diff * t);

		// モードごとの当たり判定
		if (selectionMode_ == SelectionMode::Vertex)
		{
			for (int i = 0; i < 4; ++i)
			{
				Vector3 v = poly.vertices[i];
				Vector3 diff = hitPoint - v;
				// Y軸を含めた完全な3D距離の2乗に変更
				float sqrDist = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

				if (sqrDist < minSqrDist)
				{
					minSqrDist = sqrDist;
					closestPolyId = poly.id;
					closestItemIdx = i;
				}
			}
		}
		else if (selectionMode_ == SelectionMode::Edge)
		{
			for (int i = 0; i < 4; ++i)
			{
				Vector3 p0 = poly.vertices[i];
				Vector3 p1 = poly.vertices[(i + 1) % 4];

				Vector3 ab = p1 - p0;
				Vector3 ap = hitPoint - p0;

				float tEdge = (ap.x * ab.x + ap.y * ab.y + ap.z * ab.z) /
					(ab.x * ab.x + ab.y * ab.y + ab.z * ab.z);
				tEdge = std::max(0.0f, std::min(1.0f, tEdge));

				Vector3 closest(p0.x + tEdge * ab.x, p0.y + tEdge * ab.y, p0.z + tEdge * ab.z);
				Vector3 diff = hitPoint - closest;

				// Y軸を含めた完全な3D距離の2乗
				float sqrDist = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

				if (sqrDist < minSqrDist)
				{
					minSqrDist = sqrDist;
					closestPolyId = poly.id;
					closestItemIdx = i;
				}
			}
		}
		else if (selectionMode_ == SelectionMode::Polygon)
		{
			// 面選択: 交点がポリゴン内にあるか判定
			if (poly.IsPointInside(hitPoint))
			{
				// 複数のポリゴンが重なっている場合、一番手前（tが小さい）ものを優先
				if (t < closestHitT)
				{
					closestHitT = t;
					closestPolyId = poly.id;
					closestItemIdx = 0;
				}
			}
		}
	}

	// Shiftキーが押されているかどうかをチェック（複数選択のため）
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

			if (!alreadySelected) selectedItems_.push_back(newItem);
			else selectedItems_.erase(it);
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

	// 押し出す距離を設定（ここでは2.0f単位で押し出す）
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

	// 頂点の並び順を決定して新しいポリゴンにセット
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

/// @brief 選択されているポリゴンを有効化する
/// @param placementList 
/// @param isDirty 
void StageEditorNavMeshController::ActivePolygon(std::vector<PlacementData>& placementList, bool& isDirty)
{
	for (auto& item : selectedItems_)
	{
		NavPolygon* poly = navMesh_->GetMutablePolygon(item.polygonId);
		if (poly)
		{
			poly->isActive = !poly->isActive;
			isDirty = true;
		}
	}
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

		Vector4 highlightColor = Vector4(1.0f, 1.0f, 0.0f, 0.5f); // 半透明の黄色
		if (!poly->isActive) highlightColor = Vector4(0.0f, 1.0f, 1.0f, 0.5f); // 非アクティブなポリゴンは半透明の緑色

		if (selectionMode_ == SelectionMode::Vertex)
		{
			Vector3 v = poly->vertices[item.itemIndex];
			Vector3 renderV = Vector3(v.x, v.y + 0.05f, v.z);

			// 頂点の強調表示
			engine_->DrawDebugLine3D(renderV, renderV + Vector3(0, 0.5f, 0), highlightColor);
		}
		else if (selectionMode_ == SelectionMode::Edge)
		{
			Vector3 v0 = poly->vertices[item.itemIndex];
			Vector3 v1 = poly->vertices[(item.itemIndex + 1) % 4];
			Vector3 renderV0 = Vector3(v0.x, v0.y + 0.01f, v0.z);
			Vector3 renderV1 = Vector3(v1.x, v1.y + 0.01f, v1.z);

			// 辺の強調表示
			engine_->DrawDebugLine3D(renderV0, renderV1, highlightColor);
		}
		else if (selectionMode_ == SelectionMode::Polygon)
		{
			// 面の強調表示（半透明の三角形などで塗りつぶす）
			Vector3 v0 = poly->vertices[0]; Vector3 v1 = poly->vertices[1];
			Vector3 v2 = poly->vertices[2]; Vector3 v3 = poly->vertices[3];
			v0.y += 0.01f; v1.y += 0.01f; v2.y += 0.01f; v3.y += 0.01f;

			// 半透明の黄色で塗りつぶす
			engine_->DrawDebugTriangle3D(v0, v1, v2, highlightColor);
			engine_->DrawDebugTriangle3D(v0, v2, v3, highlightColor);
		}
	}
}
