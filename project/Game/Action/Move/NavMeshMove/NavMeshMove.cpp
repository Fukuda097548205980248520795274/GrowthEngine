#include "NavMeshMove.h"
#include "Entity/Character/Character.h"

namespace {
    constexpr float kRepathDistanceSq = 1.0f * 1.0f; // 再計算するターゲットの移動距離の二乗
    constexpr float kMinMoveDistance = 0.001f;       // 移動入力を行う最小距離
}

/// @brief コンストラクタ
/// @param character 
/// @param initData
NavMeshMove::NavMeshMove(Character* character, const NavMeshMoveInitData& initData)
	: Move(character, initData.isDash), stopDistanceSq_(initData.stopDistance* initData.stopDistance), moveSpeed_(initData.moveSpeed)
{
}

/// @brief 実行
void NavMeshMove::Exec()
{
    if (IsExec()) return;

    // 基底クラス
    Move::Exec();

    // ターゲットを取得する
    Character* target = owner_->GetLockOnTarget();
	const NavMesh* navMesh = owner_->GetNavMesh();

	// ターゲットがいない場合は移動を停止して終了する
    if (!target || !navMesh)
    {
        Exit();
        return;
    }

	// ターゲットの位置を保存（追従用）
	lastTargetPosition_ = target->GetWorldPosition();

    // 初回の経路探索を実行
    path_ = navMesh->FindPath(owner_->GetWorldPosition(), target->GetWorldPosition());
    currentPathIndex_ = 0;

    // 経路が見つからなかった場合
    if (path_.empty())
    {
        // すでに目的地（停止距離内）にいるかチェック
        Vector3 toTarget = target->GetWorldPosition() - owner_->GetWorldPosition();
        toTarget.y = 0.0f;
        if ((toTarget.x * toTarget.x + toTarget.z * toTarget.z) <= stopDistanceSq_)
        {
            // すでに到着しているので成功終了
            Action::Update();
            return;
        }

        // 離れているのに経路がない場合は移動不能として失敗終了
        Exit();
    }
}

/// @brief 更新処理
void NavMeshMove::Update()
{
    // 動けない状態や回避中の場合は移動を停止して終了
    if (owner_->IsJustAvoided() || owner_->IsGrabbing() || owner_->IsIncapacitated())
    {
        Exit();
        return;
    }

    // ターゲットを取得する
    Character* target = owner_->GetLockOnTarget();
	NavMesh* navMesh = const_cast<NavMesh*>(owner_->GetNavMesh());

    // ターゲットがいない場合は移動を停止して終了する
    if (!target || !navMesh)
    {
        Exit();
        return;
    }

    // ターゲットキャラクターを追従している場合、ターゲットが一定距離（例: 1m）動いたら経路を再計算（リパス）
    if (target)
    {
        Vector3 currentTargetPos = target->GetWorldPosition();
        float dx = currentTargetPos.x - lastTargetPosition_.x;
        float dz = currentTargetPos.z - lastTargetPosition_.z;

        if ((dx * dx + dz * dz) > kRepathDistanceSq)
        {
            // ターゲットが1m以上動いていたら経路を再計算
            lastTargetPosition_ = currentTargetPos;

            // 経路を再計算してインデックスをリセット
            path_ = navMesh->FindPath(owner_->GetWorldPosition(), target->GetWorldPosition() );
            currentPathIndex_ = 0;
        }
    }

	// 経路がない場合は移動を停止して終了する
    if (path_.empty() || currentPathIndex_ >= path_.size())
    {
        Exit();
        return;
    }

    // 現在目指すべき中継点（ウェイポイント）を取得
    Vector3 waypoint = path_[currentPathIndex_];
    Vector3 toWaypoint = waypoint - owner_->GetWorldPosition();
    toWaypoint.y = 0.0f;

	// 中継点までの距離の二乗を計算
    float distSq = toWaypoint.x * toWaypoint.x + toWaypoint.z * toWaypoint.z;

    // 現在の中継点が「最後のゴール地点」かどうか
    bool isLastWaypoint = (currentPathIndex_ == path_.size() - 1);

	// 中継点に到達したとみなす距離を選択（最後のゴール地点なら停止距離、それ以外はウェイポイントの半径）
    float checkRadiusSq = isLastWaypoint ? stopDistanceSq_ : waypointRadiusSq_;

	// 中継点に到達したかどうかをチェック
    if (distSq <= checkRadiusSq)
    {
        // 最終目的地に到着
        if (isLastWaypoint)
        {
            Action::Update(); // 成功終了
            return;
        }
        else
        {
            // 途中の中継点を通ったので、次のインデックスへ進める
            currentPathIndex_++;
            if (currentPathIndex_ < path_.size())
            {
                // 次の中継点へのベクトルを再計算
                waypoint = path_[currentPathIndex_];
                toWaypoint = waypoint - owner_->GetWorldPosition();
                toWaypoint.y = 0.0f;
                distSq = toWaypoint.x * toWaypoint.x + toWaypoint.z * toWaypoint.z;
            }
        }
    }

    // 中継点に向けて移動入力を与える
    float len = std::sqrt(distSq);
    if (len > kMinMoveDistance)
    {
        Vector2 moveDirection = Vector2(toWaypoint.x / len, toWaypoint.z / len);
        owner_->SetMoveInputXZ(moveDirection, moveSpeed_);
    }
    else
    {
        owner_->MoveStop();
    }
}