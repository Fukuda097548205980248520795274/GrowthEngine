#include "NavMeshLeaderMove.h"
#include "Entity/Character/Player/Player.h"
#include "Entity/Character/NPC/NPC.h"

namespace 
{
    // 再計算するターゲットの移動距離の二乗
    constexpr float kReDistanceSq = 1.0f * 1.0f;

    // 移動入力を行う最小距離
    constexpr float kMinMoveDistance = 0.001f;
}

/// @brief コンストラクタ
/// @param character 
/// @param initData
NavMeshLeaderMove::NavMeshLeaderMove(Character* character, const NavMeshLeaderMoveInitData& initData)
    : Move(character, initData.isDash), stopDistanceSq_(initData.stopDistance* initData.stopDistance), moveSpeed_(initData.moveSpeed)
{
}

/// @brief 実行
void NavMeshLeaderMove::Exec()
{
	// ブレークポイントのチェック
	BreakpointOnExec();

    if (IsExec()) return;

    // 基底クラス
    Move::Exec();

    // リーダーを取得する
    auto characters = owner_->GetCharacters();
    std::vector<std::pair<Character*, float>> leaderCandidates;

    for (auto& character : characters)
    {
        // 自分自身は除外する
        if (owner_ == character) continue;

        // 味方の場合はプレイヤーがリーダー 敵の場合はボスがリーダー になるようにする
        if (owner_->GetCharacterTag() == CharacterTag::Ally && character->GetCharacterTag() == CharacterTag::Player ||
            owner_->GetCharacterTag() == CharacterTag::EnemyNormal && character->GetCharacterTag() == CharacterTag::EnemyBoss)
        {
            // リーダー候補として追加する
            leaderCandidates.emplace_back(character, (character->GetPosition() - owner_->GetPosition()).LengthSq());
        }
    }

    // リーダーがいない場合は移動を停止して終了する
    if (leaderCandidates.empty())
    {
        Exit();
        return;
    }

    // 距離が最も近いリーダーを選択する
    std::sort(leaderCandidates.begin(), leaderCandidates.end(),
        [](const std::pair<Character*, float>& a, const std::pair<Character*, float>& b) {return a.second < b.second;});

    leader_ = leaderCandidates.front().first;

	// ナビゲーションメッシュを取得する
    const NavMesh* navMesh = owner_->GetNavMesh();

	// ナビゲーションメッシュがない場合は移動を停止して終了する
    if (!navMesh)
    {
        Exit();
        return;
    }

    // リーダーがの位置を保存
    lastLeaderPosition_ = leader_->GetWorldPosition();

	// すでに目的地（停止距離内）にいるかチェック
	Vector3 toLeader = leader_->GetWorldPosition() - owner_->GetWorldPosition();
	if (toLeader.LengthSq() <= stopDistanceSq_ * stopBeforeDistanceSq_)
	{
		Action::Update();
		return;
	}

    // 初回の経路探索を実行
    path_ = navMesh->FindPath(owner_->GetWorldPosition(), leader_->GetWorldPosition());

    // 経路の中継点インデックスをリセット
    currentPathIndex_ = 0;

    // 経路が見つからなかった場合
    if (path_.empty())
    {
        // すでに目的地（停止距離内）にいるかチェック
        Vector3 toLeader = leader_->GetWorldPosition() - owner_->GetWorldPosition();
        toLeader.y = 0.0f;
        if ((toLeader.x * toLeader.x + toLeader.z * toLeader.z) <= stopDistanceSq_)
        {
            // すでに到着しているので成功終了
            Action::Update();
            return;
        }

        // 自分の最も近いNavMesh上の点を取得
        std::optional<Vector3> myNearestPos = navMesh->GetNearestPoint(owner_->GetWorldPosition(), 1.5f);

        // リーダーの最も近いNavMesh上の点を取得
        std::optional<Vector3> leaderNearestPos = navMesh->GetNearestPoint(leader_->GetWorldPosition(), 1.5f);

        if (myNearestPos != std::nullopt && leaderNearestPos != std::nullopt)
        {
            // 自分と最も近い点が離れている（＝自分がNavMesh外にいる）場合
            if ((owner_->GetWorldPosition() - myNearestPos.value()).LengthSq() > kMinMoveDistance)
            {
                // 復帰ポイントを仮の目的地にして経路を再設定（または直接そこへ向かう）
                path_ = { myNearestPos.value() };
                currentPathIndex_ = 0;

                // ダッシュ状態に遷移する
                auto stateMachine = owner_->GetStateMachine();
                if (isDash_)
                {
                    stateMachine->ChangeState("Dash");
                }

                return; // 復帰へ向かう
            }

            // リーダーがNavMesh外にいる場合は、リーダーの最近傍点への経路を探す
            path_ = navMesh->FindPath(owner_->GetWorldPosition(), leaderNearestPos.value());
            currentPathIndex_ = 0;
        }

        // それでもダメなら失敗終了
        if (path_.empty())
        {
            Exit();
            return;
        }
    }

    // ダッシュ状態に遷移する
    auto stateMachine = owner_->GetStateMachine();
    if (isDash_)
    {
        stateMachine->ChangeState("Dash");
    }
}

/// @brief 更新処理
void NavMeshLeaderMove::Update()
{
	// ブレークポイントのチェック
	BreakpointOnUpdate();

    // 動けない状態や回避中の場合は移動を停止して終了
    if (owner_->IsJustAvoided() || owner_->IsGrabbing() || owner_->IsIncapacitated())
    {
        Exit();
        return;
    }

	// ナビゲーションメッシュを取得する
    NavMesh* navMesh = const_cast<NavMesh*>(owner_->GetNavMesh());

    // リーダーががいない場合は移動を停止して終了する
    if (!leader_ || !navMesh)
    {
        Exit();
        return;
    }

	// リーダーがいる場合は、リーダーの位置をチェックして経路を再計算する
    if (leader_)
    {
		// リーダーの現在位置を取得
        Vector3 currentLeaderPos = leader_->GetWorldPosition();

		// すでに目的地（停止距離内）にいるかチェック
        Vector3 toLeader = leader_->GetWorldPosition() - owner_->GetWorldPosition();
        toLeader.y = 0.0f;
        if ((toLeader.x * toLeader.x + toLeader.z * toLeader.z) <= stopDistanceSq_)
        {
            // すでに到着しているので成功終了
            Action::Update();
            return;
        }

		// リーダーの位置が前回の位置からどれだけ動いたかを計算
        float dx = currentLeaderPos.x - lastLeaderPosition_.x;
        float dz = currentLeaderPos.z - lastLeaderPosition_.z;

        if ((dx * dx + dz * dz) > kReDistanceSq)
        {
            // リーダーがが1m以上動いていたら経路を再計算
            lastLeaderPosition_ = currentLeaderPos;

            // 経路を再計算してインデックスをリセット
            path_ = navMesh->FindPath(owner_->GetWorldPosition(), leader_->GetWorldPosition());
            currentPathIndex_ = 0;

            // 経路が見つからなかった場合
            if (path_.empty())
            {
                // 自分の最も近いNavMesh上の点を取得
                std::optional<Vector3> myNearestPos = navMesh->GetNearestPoint(owner_->GetWorldPosition(), 1.5f);

                // リーダーの最も近いNavMesh上の点を取得
                std::optional<Vector3> leaderNearestPos = navMesh->GetNearestPoint(leader_->GetWorldPosition(), 1.5f);

                if (myNearestPos != std::nullopt && leaderNearestPos != std::nullopt)
                {
                    // 自分と最も近い点が離れている（＝自分がNavMesh外にいる）場合
                    if ((owner_->GetWorldPosition() - myNearestPos.value()).LengthSq() > kMinMoveDistance)
                    {
                        // 復帰ポイントを仮の目的地にして経路を再設定（または直接そこへ向かう）
                        path_ = { myNearestPos.value() };
                        currentPathIndex_ = 0;
                        return; // 復帰へ向かう
                    }

                    // リーダーがNavMesh外にいる場合は、リーダーの最近傍点への経路を探す
                    path_ = navMesh->FindPath(owner_->GetWorldPosition(), leaderNearestPos.value());
                    currentPathIndex_ = 0;
                }

                // それでもダメなら失敗終了
                if (path_.empty())
                {
                    Exit();
                    return;
                }
            }
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

    // 現在の中継点が 最後のゴール地点 かどうか
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

    // ダッシュ状態に遷移する
    if (isDash_ && !owner_->IsStance())
    {
        owner_->GetStateMachine()->ChangeState("Dash");
    }

    
    float len = std::sqrt(distSq);
    if (len > kMinMoveDistance)
    {
        // プレイヤーでなく、攻撃シーケンス中でないNPCの場合は、分離ベクトルを考慮して移動する
        if (!owner_->IsPlayer() && !owner_->IsInAttackSequence())
        {
            NPC* npc = static_cast<NPC*>(owner_);

            // 中継点へ向かうベースの移動ベクトル（正規化済み）
            Vector2 moveDirection = Vector2(toWaypoint.x / len, toWaypoint.z / len);

            // 分離ベクトルの計算
            Vector2 separationDirection = npc->CalculateSeparationVector();



            // ベースの移動ベクトルと分離ベクトルの内積を計算
            float dot = moveDirection.x * separationDirection.x + moveDirection.y * separationDirection.y;

            // 回避タイマーの減衰
            if (avoidanceTimer_ > 0.0f)
            {
                avoidanceTimer_ -= engine_->GetDeltaTime() * engine_->GetTimeScale();
            }
            else
            {
                // タイマーが切れたら回避方向をリセット
                avoidanceSide_ = 0.0f;
            }

            // 内積が一定以上マイナス（＝ 前にキャラクターがいる）場合
            if (dot < -0.1f)
            {
                Vector2 rightDir(moveDirection.y, -moveDirection.x); // 右方向

                // まだ回避方向が決まっていない場合のみ、どちらに避けるか新しく決定する
                if (avoidanceSide_ == 0.0f)
                {
                    float rightDot = rightDir.x * separationDirection.x + rightDir.y * separationDirection.y;
                    avoidanceSide_ = (rightDot >= 0.0f) ? 1.0f : -1.0f;
                    avoidanceTimer_ = 0.4f; // 0.4秒間はこの避ける向きを維持する
                }

                // 真横ではなく「斜め前」に滑らかにコースをずらすベクトルを作成
                float sideWeight = (avoidanceSide_ >= 1.0f) ? 0.6f : -0.6f;
                Vector2 slantDir = Vector2(moveDirection.x + rightDir.x * sideWeight, moveDirection.y + rightDir.y * sideWeight).Normalize();

                // 分離ベクトルを「斜め前への迂回ベクトル」に上書き
                separationDirection = slantDir;
            }
            else
            {
                // 正面に障害がない場合は通常の分離（離れる力）のまま
                avoidanceSide_ = 0.0f;
            }



            // ベクトルの合成（ベース方向 ＋ 分離方向 × 重み）
            Vector2 finalDirection(
                moveDirection.x + separationDirection.x * npc->GetSeparationWeight(),
                moveDirection.y + separationDirection.y * npc->GetSeparationWeight());

            // 合成した「目標となるベクトル」を正規化する
            finalDirection = finalDirection.Normalize();

            // 前回のベクトルから目標ベクトルへLerp（補間）する
            float lerpFactor = 0.15f;
            currentMoveDirection_.x = currentMoveDirection_.x + (finalDirection.x - currentMoveDirection_.x) * lerpFactor;
            currentMoveDirection_.y = currentMoveDirection_.y + (finalDirection.y - currentMoveDirection_.y) * lerpFactor;

            // 補間した最終ベクトルをもう一度正規化して保存・入力
            currentMoveDirection_ = currentMoveDirection_.Normalize();

            // 移動入力を設定
            owner_->SetMoveInputXZ(currentMoveDirection_, moveSpeed_);
        }
        else
        {
            Vector2 moveDirection = Vector2(toWaypoint.x / len, toWaypoint.z / len);
            owner_->SetMoveInputXZ(moveDirection, moveSpeed_);
        }
    }
    else
    {
        owner_->MoveStop();
    }
}