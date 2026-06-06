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

    // 選択されている辺の情報を保持する構造体
    struct SelectedEdge
    {
        int polygonId = -1; // 選択中のポリゴンID (-1で未選択)
        int edgeIndex = -1; // 選択中の辺のインデックス (0～3)
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


private:

	/// @brief マウスからレイキャストを飛ばして、NavMeshの辺を選択する処理
	Engine::Collision3D::Ray RaycastFromMouse();

	/// @brief 点ptと線分abの距離の二乗を計算する関数
    /// @param pt 
    /// @param a 
    /// @param b 
    /// @return 
    float SqrDistancePointToSegment(const Vector3& pt, const Vector3& a, const Vector3& b);

	/// @brief 選択されている辺を押し出す
    void SelectNavMeshEdge();


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
    void MoveSelectedEdge(const Vector3& moveDelta);

    // NavMeshへのポインタ（初期化時にシーンなどから取得してセットしてください）
    NavMesh* navMesh_ = nullptr;

    /// @brief 選択された辺
    SelectedEdge selectedEdge_;

    // ドラッグ状態の管理
    bool isDraggingEdge_ = false;

    // 前回のレイと床の交点
    Vector3 previousHitPoint_;


private:

	// 押し出しの入力キー
	std::unique_ptr<InputKey> extrudeEdge_ = nullptr;
};

