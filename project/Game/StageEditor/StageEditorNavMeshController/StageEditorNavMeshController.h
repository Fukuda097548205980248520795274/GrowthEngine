#pragma once
#include <vector>
#include "NavMesh/NavMesh.h"
#include "GrowthEngine.h"
#include "StageEditor/StageEditorHistory/StageEditorHistory.h"
#include "StageEditor/StageData/StageData.h"

class StageEditorNavMeshController
{
public:

	// エディタの選択モード
	enum class SelectionMode
	{
		// 頂点選択モード
		Vertex,

		// 辺選択モード
		Edge,

		// 面選択モード
		Polygon
	};

	// 選択されている辺の情報を保持する構造体
	struct SelectedItem
	{
		// 選択されているポリゴンのID
		int polygonId = -1;

		// 頂点選択モードなら頂点インデックス、辺選択モードなら辺インデックス、面選択モードなら無視される
		int itemIndex = -1;
	};


public:

	/// @brief コンストラクタ
	/// @param navMesh 
	/// @param history 
	StageEditorNavMeshController(NavMesh* navMesh, StageEditorHistory* history) : navMesh_(navMesh), history_(history) {}

	/// @brief デストラクタ
	~StageEditorNavMeshController() = default;

	/// @brief 更新処理
	/// @param placementList 
	/// @param isDirty 
	void Update(std::vector<PlacementData>& placementList, bool& isDirty);

	/// @brief デバッグ用描画処理
	void DrawDebug();

	/// @brief 辺を押し出すことができるか
	/// @return 
	bool CanExtrudeSelectedEdge() const { return (selectionMode_ == SelectionMode::Edge && selectedItems_.size() == 1); }

	/// @brief 辺を繋ぐ（ブリッジ）ことができるか
	/// @return 
	bool CanBridgeSelectedEdges() const;

	/// @brief 選択されている要素のリストを取得
	/// @return 
	const std::vector<SelectedItem>& GetSelectedItems() const { return selectedItems_; }

	/// @brief 現在の選択モードを取得
	/// @return 
	SelectionMode GetSelectionMode() const { return selectionMode_; }


private:

	// エンジンのインスタンス
	const GrowthEngine* engine_ = GrowthEngine::GetInstance();

	// ナビメッシュへのポインタ
	NavMesh* navMesh_ = nullptr;

	// 履歴管理クラスへのポインタ
	StageEditorHistory* history_ = nullptr;

	// 現在の選択モード
	SelectionMode selectionMode_ = SelectionMode::Edge;

	// 現在選択されている辺のリスト
	std::vector<SelectedItem> selectedItems_;

	// ドラッグ状態の管理
	bool isDraggingItem_ = false;

	// 前回のレイと床の交点
	Vector3 previousHitPoint_{};


private:

	/// @brief マウスからレイキャストを飛ばして、NavMeshの辺を選択する処理
	Engine::Collision3D::Ray RaycastFromMouse();

	/// @brief レイとXZ平面（y=planeY）との交点を計算する関数
	/// @param ray 
	/// @param planeY 
	/// @return 
	Vector3 GetRayIntersectionWithPlane(const Engine::Collision3D::Ray& ray, float planeY = 0.0f);

	/// @brief ナビゲーションメッシュの選択
	void SelectNavMeshItem();

	/// @brief 選択されている辺を押し出す
	void ExtrudeSelectedEdge(std::vector<PlacementData>& placementList, bool& isDirty);

	/// @brief 選択された辺を移動する
	/// @param moveDelta 
	void MoveSelectedItem(const Vector3& moveDelta);

	/// @brief 選択された辺を繋ぐ（ブリッジ）する
	void BridgeSelectedEdges(std::vector<PlacementData>& placementList, bool& isDirty);

	/// @brief 選択された辺を削除する
	void DeleteSelectedNavMeshElements(std::vector<PlacementData>& placementList, bool& isDirty);

	/// @brief 選択された辺をハイライト表示する
	void DrawSelectedHighlight();


private:


};

