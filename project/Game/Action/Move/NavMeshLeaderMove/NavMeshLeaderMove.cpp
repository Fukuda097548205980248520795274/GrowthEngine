#include "NavMeshLeaderMove.h"
#include "Entity/Character/Character.h"

namespace {
    constexpr float kRepathDistanceSq = 1.0f * 1.0f; // 再計算するターゲットの移動距離の二乗
    constexpr float kMinMoveDistance = 0.001f;       // 移動入力を行う最小距離
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

    // リーダーがの位置を保存（追従用）
    lastLeaderPosition_ = leader_->GetWorldPosition();

    // 初回の経路探索を実行
    path_ = navMesh->FindPath(owner_->GetWorldPosition(), leader_->GetWorldPosition());
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

        // 離れているのに経路がない場合は移動不能として失敗終了
        Exit();
    }
}

/// @brief 更新処理
void NavMeshLeaderMove::Update()
{
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

    // リーダーがキャラクターを追従している場合、リーダーがが一定距離（例: 1m）動いたら経路を再計算（リパス）
    if (leader_)
    {
        Vector3 currentLeaderPos = leader_->GetWorldPosition();
        float dx = currentLeaderPos.x - lastLeaderPosition_.x;
        float dz = currentLeaderPos.z - lastLeaderPosition_.z;

        if ((dx * dx + dz * dz) > kRepathDistanceSq)
        {
            // リーダーがが1m以上動いていたら経路を再計算
            lastLeaderPosition_ = currentLeaderPos;

            // 経路を再計算してインデックスをリセット
            path_ = navMesh->FindPath(owner_->GetWorldPosition(), leader_->GetWorldPosition());
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