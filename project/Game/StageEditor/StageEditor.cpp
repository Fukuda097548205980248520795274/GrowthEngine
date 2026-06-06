#include "StageEditor.h"
#include <numbers>
#include <json.hpp>
#include "Scene/GameScene/GameScene.h"

/// @brief 初期化
void StageEditor::Initialize()
{
	// ナビメッシュの取得
	navMesh_ = scene_->GetNavMesh();
    CreateInitialNavPolygon();

	// ファイルマネージャの初期化
	fileManager_ = std::make_unique<StageFileManager>("./Assets/Parameter/StageData/");

	// 履歴管理の初期化
	history_ = std::make_unique<StageEditorHistory>();

	// スペナーの初期化
	spawner_ = std::make_unique<StageSpawner>(scene_);
	spawner_->Initialize();

	// エディタUIの初期化
	editorUI_ = std::make_unique<StageEditorUI>(fileManager_.get(), spawner_.get(), history_.get());
	editorUI_->Initialize();


	// 入力キーの初期化
	inputModelChange_ = std::make_unique<InputKey>("StageEditor_ModelChange", InputState::Trigger, DIK_TAB);
	
	// 辺の選択と移動の入力キーを初期化
	pressEdge_ = std::make_unique<InputKey>("StageEditor_PressEdge", InputState::Press, DIK_LSHIFT);
	triggerEdge_ = std::make_unique<InputKey>("StageEditor_TriggerEdge", InputState::Trigger, DIK_LSHIFT);
	releaseEdge_ = std::make_unique<InputKey>("StageEditor_ReleaseEdge", InputState::Release, DIK_LSHIFT);

	// 押し出しの入力キーを初期化
	extrudeEdge_ = std::make_unique<InputKey>("StageEditor_ExtrudeEdge", InputState::Trigger, DIK_E);
}

/// @brief 更新処理
/// @param dt 
void StageEditor::Update(float dt)
{
#ifdef _DEVELOPMENT

    // ブジェクト配置モード と ナビメッシュ編集モード を切り替え
    if (inputModelChange_->IsInput())
    {
        if (currentMode_ == EditorMode::ObjectPlacement)
            currentMode_ = EditorMode::NavMeshEdit;
        else
            currentMode_ = EditorMode::ObjectPlacement;
    }

    if (currentMode_ == EditorMode::NavMeshEdit)
    {
        // 毎フレーム、マウスカーソルと床面との交点を取得
        Engine::Collision3D::Ray ray = RaycastFromMouse();
        Vector3 currentHitPoint = GetRayIntersectionWithPlane(ray, 0.0f); // 床の高さを0.0fとする

        // 左クリックが押された瞬間（選択 ＆ ドラッグ開始）
        if (triggerEdge_ && triggerEdge_->IsInput())
        {
            SelectNavMeshEdge();

            if (selectedEdge_.polygonId != -1)
            {
                isDraggingEdge_ = true;
                previousHitPoint_ = currentHitPoint;
            }
        }
        // 左クリックが押し続けられている間（ドラッグ中）
        else if (pressEdge_ && pressEdge_->IsInput() && isDraggingEdge_)
        {
            // 前回の交点から現在の交点への移動ベクトルを計算
            Vector3 moveDelta = currentHitPoint - previousHitPoint_;

            // 少しでもマウスが動いていれば辺を動かす
            if (std::abs(moveDelta.x) > 0.001f || std::abs(moveDelta.z) > 0.001f)
            {
                MoveSelectedEdge(moveDelta);
            }

            // 次のフレームのために交点を更新
            previousHitPoint_ = currentHitPoint;
        }
        // 左クリックが離された瞬間（ドラッグ終了）
        else if (releaseEdge_ && releaseEdge_->IsInput())
        {
            isDraggingEdge_ = false;
        }

		// 押し出しキーが押された瞬間
		if (extrudeEdge_ && extrudeEdge_->IsInput())
		{
			ExtrudeSelectedEdge();
		}
    }

#endif
}

/// @brief 描画処理（デバッグ用）
void StageEditor::DrawUI()
{
#ifdef _DEVELOPMENT

	editorUI_->DrawAssetWindow(placementList_, currentFileName_, isPlaying_, navMesh_);
	editorUI_->DrawUI(placementList_, currentFileName_, isPlaying_, navMesh_);
	editorUI_->DrawObjectListWindow(placementList_, navMesh_);

#endif
}


/// @brief マウスからレイキャストを飛ばして、NavMeshの辺を選択する処理
Engine::Collision3D::Ray StageEditor::RaycastFromMouse()
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

/// @brief 点ptと線分abの距離の二乗を計算する関数
/// @param pt 
/// @param a 
/// @param b 
/// @return 
float StageEditor::SqrDistancePointToSegment(const Vector3& pt, const Vector3& a, const Vector3& b)
{
    Vector3 ab = b - a;
    Vector3 ap = pt - a;

    // 線分上での投影位置を計算（0.0～1.0にクランプ）
    float t = (ap.x * ab.x + ap.y * ab.y + ap.z * ab.z) / (ab.x * ab.x + ab.y * ab.y + ab.z * ab.z);
    t = std::max(0.0f, std::min(1.0f, t));

    // 線分上の最も近い点
    Vector3 closest(a.x + t * ab.x, a.y + t * ab.y, a.z + t * ab.z);

    // 距離の2乗を返す
    Vector3 diff = pt - closest;
    return (diff.x * diff.x + diff.y * diff.y + diff.z * diff.z);
}

/// @brief 選択されている辺を押し出す
void StageEditor::SelectNavMeshEdge()
{
    if (!navMesh_) return;

	// マウスからレイを飛ばす
    Engine::Collision3D::Ray ray = RaycastFromMouse();

    // レイが下を向いていない場合は無視（空をクリックしている）
    if (ray.diff.y >= 0.0f) return;

	// レイとXZ平面（y=0）との交点を計算
    float distanceToFloor = (0.0f - ray.start.y) / ray.diff.y;
    Vector3 hitPoint = ray.start + (ray.diff * distanceToFloor);

    int closestPolyId = -1;
    int closestEdgeIdx = -1;
    float minSqrDist = 1.0f; // クリック判定のしきい値（距離の2乗。1.0fなら半径1m以内の辺を許容）

    // 全てのポリゴンの全ての辺をチェックし、交点(hitPoint)に最も近いものを探す
    for (const auto& poly : navMesh_->GetPolygons()) // ※NavMeshにGetPolygons()を実装してください
    {
        for (int i = 0; i < 4; ++i)
        {
            Vector3 v0 = poly.vertices[i];
            Vector3 v1 = poly.vertices[(i + 1) % 4];

            // 点(hitPoint)と辺(v0-v1)の距離を計算
            float sqrDist = SqrDistancePointToSegment(hitPoint, v0, v1);

            if (sqrDist < minSqrDist)
            {
                minSqrDist = sqrDist;
                closestPolyId = poly.id;
                closestEdgeIdx = i;
            }
        }
    }

    // 最も近い辺が見つかったら選択状態を更新
    if (closestPolyId != -1)
    {
        selectedEdge_.polygonId = closestPolyId;
        selectedEdge_.edgeIndex = closestEdgeIdx;
    }
    else
    {
        // 何も無いところをクリックしたら選択解除
        selectedEdge_.polygonId = -1;
        selectedEdge_.edgeIndex = -1;
    }
}


/// @brief 選択されている辺を押し出す
void StageEditor::ExtrudeSelectedEdge()
{
    if (selectedEdge_.polygonId == -1 || navMesh_ == nullptr) return;

    NavPolygon* poly = navMesh_->GetMutablePolygon(selectedEdge_.polygonId);
    if (!poly) return;

    // すでに他のポリゴンと繋がっている辺からは押し出し不可にする
    if (poly->neighborIds[selectedEdge_.edgeIndex] != -1) return;

    int eIdx = selectedEdge_.edgeIndex;
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
    selectedEdge_.polygonId = newPoly.id;
    selectedEdge_.edgeIndex = 2;
}

/// @brief 初期のナビメッシュを作成する
void StageEditor::CreateInitialNavPolygon()
{
    if (!navMesh_) return;

    // もしすでにポリゴンがあるなら、二重に作らないようにする場合はここでリターン
    // if (!navMesh_->GetPolygons().empty()) return;

    NavPolygon poly;
    poly.id = navMesh_->GenerateNewPolygonId();

    // 原点付近に 10x10 の四角形を生成 
    float baseHeight = 0.1f;
    poly.vertices = {
        Vector3(-5.0f, baseHeight,  5.0f), // 左上
        Vector3(5.0f, baseHeight,  5.0f), // 右上
        Vector3(5.0f, baseHeight, -5.0f), // 右下
        Vector3(-5.0f, baseHeight, -5.0f)  // 左下
    };
    poly.neighborIds = { -1, -1, -1, -1 }; // 初期はどこも隣接していない

    navMesh_->AddPolygon(poly);

    // 生成したら自動的にそのポリゴンの辺0を選択状態にする
    selectedEdge_.polygonId = poly.id;
    selectedEdge_.edgeIndex = 0;
}

/// @brief レイとXZ平面（y=planeY）との交点を計算する関数
/// @param ray 
/// @param planeY 
/// @return 
Vector3 StageEditor::GetRayIntersectionWithPlane(const Engine::Collision3D::Ray& ray, float planeY)
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
void StageEditor::MoveSelectedEdge(const Vector3& moveDelta)
{
    if (selectedEdge_.polygonId == -1 || !navMesh_) return;

    NavPolygon* targetPoly = navMesh_->GetMutablePolygon(selectedEdge_.polygonId);
    if (!targetPoly) return;

    // 移動前の辺の2つの頂点座標を取得しておく
    int eIdx = selectedEdge_.edgeIndex;
    Vector3 oldV0 = targetPoly->vertices[eIdx];
    Vector3 oldV1 = targetPoly->vertices[(eIdx + 1) % 4];

    // 新しい座標を計算
    Vector3 newV0 = oldV0 + moveDelta;
    Vector3 newV1 = oldV1 + moveDelta;

    // --- 共有頂点の同期更新 ---
    for (auto& poly : navMesh_->GetMutablePolygons())
    {
        for (int i = 0; i < 4; ++i)
        {
            // 頂点が oldV0 とほぼ同じ位置にあれば newV0 に更新
            Vector3 diff0 = poly.vertices[i] - oldV0;
            if ((diff0.x * diff0.x + diff0.y * diff0.y + diff0.z * diff0.z) < 0.01f)
            {
                poly.vertices[i] = newV0;
            }
            // 頂点が oldV1 とほぼ同じ位置にあれば newV1 に更新
            else
            {
                Vector3 diff1 = poly.vertices[i] - oldV1;
                if ((diff1.x * diff1.x + diff1.y * diff1.y + diff1.z * diff1.z) < 0.01f)
                {
                    poly.vertices[i] = newV1;
                }
            }
        }
    }
}