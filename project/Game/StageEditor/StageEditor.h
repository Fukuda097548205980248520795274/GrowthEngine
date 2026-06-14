#pragma once
#include "StageData/StageData.h"
#include "StageEditorUI/StageEditorUI.h"
#include "StageFileManager/StageFileManager.h"
#include "StageSpawner/StageSpawner.h"
#include "StageEditorHistory/StageEditorHistory.h"

class GameScene;
class NavMesh;

class StageEditor
{
public:

	// エディタの選択モード
	enum class SelectionMode
	{
		Vertex, // 1: 点選択
		Edge,   // 2: 辺選択
		Polygon // 3: 面選択
	};

	// 選択されている辺の情報を保持する構造体
	struct SelectedItem
	{
		int polygonId = -1; // 選択中のポリゴンID (-1で未選択)
		int itemIndex = -1; // 頂点選択なら頂点インデックス、辺選択なら辺インデックス、面選択なら-1
	};


public:

	/// @brief コンストラクタ
	/// @param scene 
	StageEditor(GameScene* scene) : scene_(scene) {}

	/// @brief デストラクタ
	~StageEditor() = default;

	/// @brief 初期化
	void Initialize();

	/// @brief 更新処理
	/// @param dt 
	void Update(float dt);

	/// @brief 描画処理（デバッグ用）
	void DrawUI();

	/// @brief オブジェクトを配置する
	/// @param data 
	void SpawnObject(PlacementData& data) { spawner_->SpawnActualEntity(data); }

	/// @brief オブジェクトを削除する
	/// @param data 
	void DeleteObject(PlacementData& data) { spawner_->DeleteActualEntity(data); }

	/// @brief 配置リストにデータを追加する
	/// @param data 
	void SetPlacementList(PlacementData& data) { placementList_.push_back(data); }


private:

	/// @brief マウスからレイキャストを飛ばして、NavMeshの辺を選択する処理
	Engine::Collision3D::Ray RaycastFromMouse();

	/// @brief 点ptと線分abの距離の二乗を計算する関数
	/// @param pt 
	/// @param a 
	/// @param b 
	/// @return 
	float SqrDistancePointToSegment(const Vector3& pt, const Vector3& a, const Vector3& b);

	/// @brief ナビゲーションメッシュの選択
	void SelectNavMeshItem();


private:

	/// @brief エンジンのインスタンス
	const GrowthEngine* engine_ = GrowthEngine::GetInstance();
	
	/// @brief シーン
	GameScene* scene_ = nullptr;

	// 配置するオブジェクトのリスト
	std::vector<PlacementData> placementList_;


private:

	// エディタUIを担当するクラス
	std::unique_ptr<StageEditorUI> editorUI_;

	// ステージファイルの読み書きを担当するクラス
	std::unique_ptr<StageFileManager> fileManager_;

	// ステージ上にオブジェクトを配置・削除するためのクラス
	std::unique_ptr<StageSpawner> spawner_;

	/// @brief 編集の履歴を管理するクラス
	std::unique_ptr<StageEditorHistory> history_;


private:

	/// @brief 現在編集中のファイル名
	std::string currentFileName_ = "";

	/// @brief 実行中かどうか
	bool isPlaying_ = false;


private:

	/// @brief 選択されている辺を押し出す
	void ExtrudeSelectedEdge();

	/// @brief 初期のナビメッシュを作成する
	void CreateInitialNavPolygon();

	/// @brief レイとXZ平面（y=planeY）との交点を計算する関数
	/// @param ray 
	/// @param planeY 
	/// @return 
	Vector3 GetRayIntersectionWithPlane(const Engine::Collision3D::Ray& ray, float planeY = 0.0f);

	/// @brief 選択された辺を移動する
	/// @param moveDelta 
	void MoveSelectedItem(const Vector3& moveDelta);

	/// @brief 選択された辺を繋ぐ（ブリッジ）する
	void BridgeSelectedEdges();

	/// @brief 選択された辺を削除する
	void DeleteSelectedNavMeshElements();

	/// @brief 選択された辺をハイライト表示する
	void DrawSelectedHighlight();

	// NavMeshへのポインタ（初期化時にシーンなどから取得してセットしてください）
	NavMesh* navMesh_ = nullptr;

	/// @brief 選択された辺
	std::vector<SelectedItem> selectedItems_;

	// ドラッグ状態の管理
	bool isDraggingItem_ = false;

	// 前回のレイと床の交点
	Vector3 previousHitPoint_{};

	// 現在の選択モード
	SelectionMode selectionMode_ = SelectionMode::Edge;
};

