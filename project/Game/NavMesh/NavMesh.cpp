#include "NavMesh.h"
#include <queue>

/// @brief 位置にあるポリゴンを見つける
/// @param position 
/// @return 
int NavMesh::FindPolygonAt(const Vector3& position) const
{
	// 全ポリゴンをループして、内側にあるかチェックする
	for (const auto& poly : polygons_)
	{
		if (poly.IsPointInside(position))
		{
			return poly.id;
		}
	}

	// どのポリゴンにも乗っていなければ -1（無効値）を返す
	return -1;
}

/// @brief デバッグ用に描画する
void NavMesh::DrawDebug() const
{
#ifdef DEVELOPMENT

	for (auto& poly : polygons_)
	{
		Vector4 color = poly.isActive ? Vector4{ 1.0f, 1.0f, 0.5f, 0.1f } : Vector4{ 0.5f, 0.5f, 1.0f, 0.1f };

		// ポリゴンの頂点を線で結んで描画する
		for (int i = 0; i < 4; ++i)
		{
			Vector3 start = poly.vertices[i];
			Vector3 end = poly.vertices[(i + 1) % 4];
			engine_->DrawDebugLine3D(start, end, { 0.0f, 0.0f, 0.0f, 1.0f });
		}

		// ポリゴンの内部を半透明の色で塗りつぶす
		engine_->DrawDebugTriangle3D(poly.vertices[0], poly.vertices[1], poly.vertices[2], color);
		engine_->DrawDebugTriangle3D(poly.vertices[0], poly.vertices[2], poly.vertices[3], color);
	}

#endif
}

/// @brief スタートからゴールまでの経路を見つける
/// @param start 
/// @param end 
/// @return 
std::vector<Vector3> NavMesh::FindPath(const Vector3& start, const Vector3& end) const
{
	std::vector<Vector3> path;

	// スタートとゴールのポリゴンを特定
	int startPolyId = FindPolygonAt(start);
	int endPolyId = FindPolygonAt(end);

	// どちらかがポリゴン内にない場合は経路なし
	if (startPolyId == -1 || endPolyId == -1)
		return path;

	// スタートとゴールが同じポリゴン内なら、直接目的地を返す
	if (startPolyId == endPolyId)
	{
		path.push_back(end);
		return path;
	}

	// A*アルゴリズムのオープンリスト（優先度キュー）
	std::priority_queue<AStarNode, std::vector<AStarNode>, std::greater<AStarNode>> openList;

	// オープンリストに登録済みのノードを記録（Gコスト比較用）
	std::unordered_map<int, AStarNode> openMap;

	// クローズドリスト（探索完了したノード）
	std::unordered_map<int, AStarNode> closedList;

	// 開始ノードの初期化
	Vector3 startCenter = GetPolygonCenter(startPolyId);
	Vector3 endCenter = GetPolygonCenter(endPolyId);

	AStarNode startNode;
	startNode.polygonId = startPolyId;
	startNode.gCost = 0.0f;
	startNode.hCost = (endCenter - startCenter).Length();
	startNode.parentId = -1;

	// 開始ノードをオープンリストに追加
	openList.push(startNode);
	openMap[startPolyId] = startNode;

	bool isGoalFound = false;

	// 探索ループ
	while (!openList.empty())
	{
		// Fコストが最小のノードを取り出す
		AStarNode current = openList.top();
		openList.pop();

		// 既に探索済みの場合はスキップ
		if (closedList.count(current.polygonId)) continue;

		// クローズドリストに追加（探索完了）
		closedList[current.polygonId] = current;

		// ゴールに到達したら終了
		if (current.polygonId == endPolyId)
		{
			isGoalFound = true;
			break;
		}

		const NavPolygon* currentPoly = GetPolygon(current.polygonId);
		Vector3 currentCenter = GetPolygonCenter(current.polygonId);

		// 隣接するポリゴンを調べる
		for (int neighborId : currentPoly->neighborIds)
		{
			// 隣接ポリゴンが有効かどうかを確認
			const NavPolygon* neighbor = GetPolygon(neighborId);
			if (!neighbor || !neighbor->isActive) continue;

			// 壁
			if (neighborId == -1) continue;

			// 探索済み
			if (closedList.count(neighborId)) continue;

			Vector3 neighborCenter = GetPolygonCenter(neighborId);

			// 現在地から隣接ポリゴン中心までの距離を追加コストとする
			float moveCost = (neighborCenter - currentCenter).Length();
			float newGCost = current.gCost + moveCost;

			// 既にオープンリストにあり、今回のルートの方が遠いならスキップ
			if (openMap.count(neighborId) && newGCost >= openMap[neighborId].gCost)continue;

			// 新しいノード情報を作成してオープンリストへ
			AStarNode neighborNode;
			neighborNode.polygonId = neighborId;
			neighborNode.gCost = newGCost;

			neighborNode.hCost = (endCenter - neighborCenter).Length();
			neighborNode.parentId = current.polygonId; // 親を記録

			// 隣接ノードをオープンリストに追加
			openList.push(neighborNode);
			openMap[neighborId] = neighborNode;
		}
	}

	// 経路の復元（ゴールから親を辿ってスタートに戻る）
	if (isGoalFound)
	{
		std::vector<int> pathIds;
		int currentId = endPolyId;
		pathIds.push_back(endPolyId);

		while (currentId != startPolyId)
		{
			currentId = closedList[currentId].parentId; // 親ポリゴンへ戻る
			pathIds.push_back(currentId);
		}

		// ゴール -> スタートの順になっているので、配列を反転させる
		std::reverse(pathIds.begin(), pathIds.end());

		// ファンネルアルゴリズムで滑らかな経路を計算して返す
		return SmoothPath(start, end, pathIds);
	}

	return path;
}

/// @brief IDからポリゴンを取得する
/// @param id 
/// @return 
NavPolygon* NavMesh::GetMutablePolygon(int id)
{
	for (auto& poly : polygons_)
	{
		if (poly.id == id)
		{
			return &poly;
		}
	}
	return nullptr;
}

/// @brief 新しい一意のポリゴンIDを生成する
/// @return 
int NavMesh::GenerateNewPolygonId() const
{
	int maxId = -1;
	for (const auto& poly : polygons_)
	{
		if (poly.id > maxId)
		{
			maxId = poly.id;
		}
	}
	return maxId + 1; // 現在の最大ID + 1 を返す
}

/// @brief 指定した位置に最も近い点をナビメッシュ上で取得する
/// @param position 
/// @param searchRadius 
/// @return 
std::optional<Vector3> NavMesh::GetNearestPoint(const Vector3& position, float searchRadius) const
{
	// まず現在の位置が既にNavMesh内にあるかチェックする
	if (FindPolygonAt(position) != -1)
	{
		// 既にポリゴン内にいる場合はそのまま返す
		return position;
	}

	Vector3 nearestPoint = position;
	float minDistanceSq = searchRadius * searchRadius;
	bool isFound = false;

	// 全ポリゴンの辺（エッジ）との距離を計算し、最も近い点を探す
	for (const auto& poly : polygons_)
	{
		for (int i = 0; i < 4; ++i)
		{
			Vector3 a = poly.vertices[i];
			Vector3 b = poly.vertices[(i + 1) % 4];

			// 頂点AからBへのベクトル（辺）と、Aから現在地へのベクトル
			Vector3 ab = { b.x - a.x, b.y - a.y, b.z - a.z };
			Vector3 ap = { position.x - a.x, position.y - a.y, position.z - a.z };

			// 線分ABの長さの二乗
			float abLengthSq = ab.x * ab.x + ab.y * ab.y + ab.z * ab.z;
			if (abLengthSq <= 0.0001f) continue; // 縮退した辺はスキップ

			// 線分に対する現在位置の投影位置（tパラメータ）を計算
			float t = (ap.x * ab.x + ap.y * ab.y + ap.z * ab.z) / abLengthSq;

			// tを 0.0f から 1.0f の間にクランプして、線分の範囲内に収める
			t = std::max(0.0f, std::min(1.0f, t));

			// 線分上の最も近い点を算出
			Vector3 closestPointOnEdge = {
				a.x + t * ab.x,
				a.y + t * ab.y,
				a.z + t * ab.z
			};

			// その点と現在位置との距離の二乗を計算
			Vector3 diff = {
				position.x - closestPointOnEdge.x,
				position.y - closestPointOnEdge.y,
				position.z - closestPointOnEdge.z
			};
			float distSq = diff.x * diff.x + diff.y * diff.y + diff.z * diff.z;

			// 最小距離を更新できたら記録
			if (distSq < minDistanceSq)
			{
				minDistanceSq = distSq;
				nearestPoint = closestPointOnEdge;
				isFound = true;
			}
		}
	}

	if (isFound) 
	{
		return nearestPoint;
	}
	else 
	{
		// 見つからなかったことを明示
		return std::nullopt;
	}
}

/// @brief 指定したグループIDのポリゴンを有効/無効にする
/// @param targetGroupId 
/// @param isActive 
void NavMesh::SetGroupActive(int targetGroupId, bool isActive)
{
	for (auto& poly : polygons_) 
	{
		if (poly.groupId == targetGroupId) 
			poly.isActive = isActive;
	}
}

/// @brief 指定したグループIDのポリゴンにロックする（他のグループのポリゴンは無効化する）
/// @param combatAreaGroupId 
void NavMesh::LockToCombatArea(int combatAreaGroupId)
{
	// グループIDが0（デフォルト）または指定された戦闘エリアのグループIDのポリゴンのみを有効化し、それ以外は無効化する
	for (auto& poly : polygons_)
		poly.isActive = (poly.groupId == 0 || poly.groupId == combatAreaGroupId);
}

/// @brief IDからポリゴンを取得する
/// @param id 
/// @return 
const NavPolygon* NavMesh::GetPolygon(int id) const
{
	for (const auto& poly : polygons_)
	{
		if (poly.id == id) return &poly;
	}

	return nullptr;
}

/// @brief ポリゴンの中心を計算する
/// @param polygonId 
/// @return 
const Vector3 NavMesh::GetPolygonCenter(int polygonId) const
{
	// IDからポリゴンを取得する
	const NavPolygon* poly = GetPolygon(polygonId);
	if (!poly) return Vector3(0, 0, 0);

	// 頂点の合計を計算する
	Vector3 center(0, 0, 0);
	for (int i = 0; i < 4; ++i)
	{
		center.x += poly->vertices[i].x;
		center.y += poly->vertices[i].y;
		center.z += poly->vertices[i].z;
	}

	// 頂点の平均を取る
	center.x *= 0.25f;
	center.y *= 0.25f;
	center.z *= 0.25f;

	return center;
}

/// @brief 2つのポリゴンを繋ぐポータル（共通の辺）を取得する
/// @param fromId 
/// @param toId 
/// @param outLeft 
/// @param outRight 
/// @return 
bool NavMesh::GetPortal(int fromPolyId, int toPolyId, Vector3& outLeft, Vector3& outRight) const
{
	const NavPolygon* from = GetPolygon(fromPolyId);
	if (!from) return false;

	for (int i = 0; i < 4; ++i)
	{
		// 向かいたいポリゴンと繋がっている辺を探す
		if (from->neighborIds[i] == toPolyId)
		{
			// 時計回りの定義なので、進行方向に向かって i番目が左、(i+1)番目が右になる
			outLeft = from->vertices[i];
			outRight = from->vertices[(i + 1) % 4];
			return true;
		}
	}
	return false;
}

/// @brief XZ平面での三角形の面積を計算する
/// @param a 
/// @param b 
/// @param c 
/// @return 
float NavMesh::TriArea2D(const Vector3& a, const Vector3& b, const Vector3& c) const
{
	// aから見て、bからcへ向かう時、正(>0)なら左へ曲がる、負(<0)なら右へ曲がる
	return (b.x - a.x) * (c.z - a.z) - (b.z - a.z) * (c.x - a.x);
}

/// @brief 経路をスムーズ化する（スタートとゴールはポリゴンの中心から実際の位置に補正する）
/// @param start 
/// @param end 
/// @param pathIds 
/// @return 
std::vector<Vector3> NavMesh::SmoothPath(const Vector3& start, const Vector3& end, const std::vector<int>& pathIds) const
{
	std::vector<Vector3> smoothPath;

	// ポリゴンが1つだけなら、スタートからゴールへ直進
	if (pathIds.size() <= 1)
	{
		smoothPath.push_back(end);
		return smoothPath;
	}
	
	// 全てのポータル（門）の左右の頂点をリストアップする
	std::vector<Vector3> lefts;
	std::vector<Vector3> rights;
	
	for (size_t i = 0; i < pathIds.size() - 1; ++i)
	{
		Vector3 left, right;
		if (GetPortal(pathIds[i], pathIds[i + 1], left, right))
		{
			lefts.push_back(left);
			rights.push_back(right);
		}
	}
	
	// 最後にゴール地点を「幅ゼロの門」として追加
	lefts.push_back(end);
	rights.push_back(end);
	
	// 糸引き処理の実行
	Vector3 portalApex = start;  // 糸を固定しているピンの位置
	Vector3 portalLeft = start;  // 糸が通れる左の限界
	Vector3 portalRight = start; // 糸が通れる右の限界
	
	int apexIndex = 0;
	int leftIndex = 0;
	int rightIndex = 0;
	
	for (int i = 0; i < lefts.size(); ++i)
	{
		Vector3 left = lefts[i];
		Vector3 right = rights[i];
	
		// 右端のチェック
		if (TriArea2D(portalApex, portalRight, right) >= 0.0f)
		{
			// portalApexとportalRightが同じ（初期状態）、または新しい右端が「左端」を越えていないか
			if (std::abs(portalApex.x - portalRight.x) < 0.01f && std::abs(portalApex.z - portalRight.z) < 0.01f ||
				TriArea2D(portalApex, portalLeft, right) < 0.0f)
			{
				// 右の限界を狭める
				portalRight = right;
				rightIndex = i;
			}
			else
			{
				// 左端を越えてしまった ＝ 糸が左の角に引っかかった
				smoothPath.push_back(portalLeft); // 左の角を新しい中継点として確定
				portalApex = portalLeft; // ここから糸を引き直す
				portalLeft = portalApex;
				portalRight = portalApex;
				apexIndex = leftIndex;

				i = apexIndex; // 引っかかったポータルの次から再開
				continue;
			}
		}
	
		// 右端のチェックが終わったら、左端のチェック
		if (TriArea2D(portalApex, portalLeft, left) <= 0.0f)
		{
			// portalApexとportalLeftが同じ（初期状態）、または新しい左端が「右端」を越えていないか
			if (std::abs(portalApex.x - portalLeft.x) < 0.01f && std::abs(portalApex.z - portalLeft.z) < 0.01f ||
				TriArea2D(portalApex, portalRight, left) > 0.0f)
			{
				// 左の限界を狭める
				portalLeft = left;
				leftIndex = i;
			}
			else
			{
				// 右端を越えてしまった ＝ 糸が右の角に引っかかった
				smoothPath.push_back(portalRight); // 右の角を新しい中継点として確定
				portalApex = portalRight; // ここから糸を引き直す
				portalLeft = portalApex;
				portalRight = portalApex;
				apexIndex = rightIndex;

				i = apexIndex; // 引っかかったポータルの次から再開
				continue;
			}
		}
	}
	
	// 最後にゴール地点を追加
	smoothPath.push_back(end);
	
	return smoothPath;
}