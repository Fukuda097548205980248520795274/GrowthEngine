#pragma once
#include <GrowthEngine.h>

/// @brief A*アルゴリズムのノード構造体
struct AStarNode
{
	// 対応するポリゴンのID
	int polygonId;

	// 開始点からこのノードまでのコスト
	float gCost;

	// このノードから目的地までの推定コスト
	float hCost;

	// 総コスト
	float fCost() const { return gCost + hCost; }

	// 経路復元のための親ノードのID
	int parentId;

	// 優先度キューで使用する比較演算子（fCostが小さい方が優先される）
	bool operator>(const AStarNode& other) const
	{
		return fCost() > other.fCost();
	}
};

/// @brief ナビゲーションメッシュを構成するポリゴン
struct NavPolygon
{
	// ID
	int id;

	// 頂点
	std::array<Vector3, 4> vertices;

	// 隣接するポリゴンのID
	std::array<int, 4> neighborIds;

	/// @brief 点がポリゴンの内側にあるか確認する
	/// @param point 
	/// @return 
	bool IsPointInside(const Vector3& point) const
	{
		// Y座標の平均を計算して、点がその近くにあるか確認
        float averageY = (vertices[0].y + vertices[1].y + vertices[2].y + vertices[3].y) * 0.25f;
        if (std::abs(point.y - averageY) > 2.0f)
            return false;

        // XZ平面（上から見下ろした2D）での内外判定
        for (int i = 0; i < 4; ++i)
        {
            Vector3 a = vertices[i];
            Vector3 b = vertices[(i + 1) % 4];

            // 辺のベクトル（XZ）
            float edgeX = b.x - a.x;
            float edgeZ = b.z - a.z;

            // 点へのベクトル（XZ）
            float toPointX = point.x - a.x;
            float toPointZ = point.z - a.z;

            // 2Dの外積（クロス積）を計算
            float cross2D = edgeX * toPointZ - edgeZ * toPointX;

			// 外積が正なら点は辺の外側にある
            if (cross2D > 0.0f)
                return false;
        }
        return true;
	}
};

/// @brief ナビゲーションメッシュクラス
class NavMesh
{
public:

	/// @brief 位置にあるポリゴンを見つける
	/// @param position 
	/// @return 
	int FindPolygonAt(const Vector3& position) const;

	/// @brief ポリゴンを追加する
	/// @param polygon 
	void AddPolygon(const NavPolygon& polygon) { polygons_.push_back(polygon); }

	/// @brief デバッグ用に描画する
	void DrawDebug() const;

	/// @brief スタートからゴールまでの経路を見つける
	/// @param start 
	/// @param end 
	/// @return 
	std::vector<Vector3> FindPath(const Vector3& start, const Vector3& end) const;

	/// @brief IDからポリゴンを取得する
	/// @param id 
	/// @return 
	NavPolygon* GetMutablePolygon(int id);

	/// @brief 全てのポリゴンを取得する
	/// @return 
	std::vector<NavPolygon>& GetMutablePolygons() { return polygons_; }

	/// @brief 新しい一意のポリゴンIDを生成する
	/// @return 
	int GenerateNewPolygonId() const;

	/// @brief 全てのポリゴンを取得する
	/// @return 
	std::vector<NavPolygon> GetPolygons() const { return polygons_; }

	/// @brief ナビメッシュをクリアする
	void Clear() { polygons_.clear(); }


private:

	/// @brief IDからポリゴンを取得する
	/// @param id 
	/// @return 
	const NavPolygon* GetPolygon(int id) const;

	/// @brief ポリゴンの中心を計算する
	/// @param polygonId 
	/// @return 
	const Vector3 GetPolygonCenter(int polygonId) const;

	/// @brief 2つのポリゴンを繋ぐポータル（共通の辺）を取得する
	/// @param fromId 
	/// @param toId 
	/// @param outLeft 
	/// @param outRight 
	/// @return 
	bool GetPortal(int fromPolyId, int toPolyId, Vector3& outLeft, Vector3& outRight) const;

	/// @brief XZ平面での三角形の面積を計算する
	/// @param a 
	/// @param b 
	/// @param c 
	/// @return 
	float TriArea2D(const Vector3& a, const Vector3& b, const Vector3& c) const;

	/// @brief 経路をスムーズ化する（スタートとゴールはポリゴンの中心から実際の位置に補正する）
	/// @param start 
	/// @param end 
	/// @param pathIds 
	/// @return 
	std::vector<Vector3> SmoothPath(const Vector3& start, const Vector3& end, const std::vector<int>& pathIds) const;


private:

	/// @brief エンジン
	const GrowthEngine* engine_ = GrowthEngine::GetInstance();

	// ナビゲーションメッシュを構成するポリゴンのリスト
	std::vector<NavPolygon> polygons_;

};