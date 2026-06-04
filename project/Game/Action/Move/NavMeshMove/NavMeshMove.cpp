#include "NavMeshMove.h"
#include "Entity/Character/Character.h"

/// @brief コンストラクタ
/// @param character 
/// @param initData
NavMeshMove::NavMeshMove(Character* character, const NavMeshMoveInitData& initData)
    : Move(character), stopDistanceSq_(initData.stopDistance * initData.stopDistance), moveSpeed_(initData.moveSpeed)
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
            owner_->MoveStop();
            owner_->SetCurrentMove(nullptr);
            Action::Update();
            return;
        }

        // 離れているのに経路がない場合は移動不能として失敗終了
        owner_->MoveStop();
        owner_->SetCurrentMove(nullptr);
        Exit(); // Action::Exit() 内で自動的に isFailure_ = true になります
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
        owner_->MoveStop();
        owner_->SetCurrentMove(nullptr);
        Exit();
        return;
    }

    // ターゲットキャラクターを追従している場合、ターゲットが一定距離（例: 1m）動いたら経路を再計算（リパス）
    if (target)
    {
        Vector3 currentTargetPos = target->GetWorldPosition();
        float dx = currentTargetPos.x - lastTargetPosition_.x;
        float dz = currentTargetPos.z - lastTargetPosition_.z;

		// ターゲットが1m以上動いていたら経路を再計算
		lastTargetPosition_ = currentTargetPos;

        if ((dx * dx + dz * dz) > 1.0f) // 1mの二乗
        {
            // 経路を再計算してインデックスをリセット
            path_ = navMesh->FindPath(owner_->GetWorldPosition(), target->GetWorldPosition() );
            currentPathIndex_ = 0;
        }
    }

    // 経路が空、またはすべてのチェックポイントを通過した場合
    if (path_.empty() || currentPathIndex_ >= path_.size())
    {
        owner_->MoveStop();
        owner_->SetCurrentMove(nullptr);
        Action::Update(); // 成功終了
        return;
    }

    // 現在目指すべき中継点（ウェイポイント）を取得
    Vector3 waypoint = path_[currentPathIndex_];
    Vector3 toWaypoint = waypoint - owner_->GetWorldPosition();
    toWaypoint.y = 0.0f;

    float distSq = toWaypoint.x * toWaypoint.x + toWaypoint.z * toWaypoint.z;

    // 現在の中継点が「最後のゴール地点」かどうか
    bool isLastWaypoint = (currentPathIndex_ == path_.size() - 1);

    // ゴールなら設定された停止距離、途中の中継点なら少し手前（50cm）で通過判定にする
    float checkRadiusSq = isLastWaypoint ? stopDistanceSq_ : waypointRadiusSq_;

    if (distSq <= checkRadiusSq)
    {
        if (isLastWaypoint)
        {
            // 最終目的地に到着！
            owner_->MoveStop();
            owner_->SetCurrentMove(nullptr);
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
                toWaypoint = waypoint - owner_->GetPosition();
                toWaypoint.y = 0.0f;
                distSq = toWaypoint.x * toWaypoint.x + toWaypoint.z * toWaypoint.z;
            }
        }
    }

    // 中継点に向けて移動入力を与える
    float len = std::sqrt(distSq);
    if (len > 0.001f)
    {
        Vector2 moveDirection = Vector2(toWaypoint.x / len, toWaypoint.z / len);
        owner_->SetMoveInputXZ(moveDirection, moveSpeed_);
    }
    else
    {
        owner_->MoveStop();
    }
}