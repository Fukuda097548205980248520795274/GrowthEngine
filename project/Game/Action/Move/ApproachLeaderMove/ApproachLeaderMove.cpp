#include "ApproachLeaderMove.h"
#include "Entity/Character/Character.h"

/// @brief 実行
void ApproachLeaderMove::Exec()
{
	// ブレークポイントのチェック
	BreakpointOnExec();

    // 実行済みが処理しない
    if (IsExec())return;

	// リーダーを初期化する
	leader_ = nullptr;

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

	// ダッシュ状態に遷移する
	auto stateMachine = owner_->GetStateMachine();
    if (isDash_)
    {
		stateMachine->ChangeState("Dash");
    }

    // 基底クラスで実行
    Move::Exec();
}

/// @brief 更新処理
void ApproachLeaderMove::Update()
{
	// ブレークポイントのチェック
	BreakpointOnUpdate();

	// リーダーがいない場合は移動を停止して終了する
	if (!leader_)if (leader_->IsDead() || leader_->IsFinished())
    {
        Exit();
        return;
    }

    // リーダーへのベクトルを計算する
    Vector3 toLeader = leader_->GetPosition() - owner_->GetPosition();
    toLeader.y = 0.0f;

    // リーダーとの距離の二乗を計算する
    const float distanceSq = toLeader.x * toLeader.x + toLeader.z * toLeader.z;

    // 動けない状態や回避中の場合は移動を停止して終了する
    if (owner_->IsJustAvoided() || owner_->IsGrabbing() || owner_->IsIncapacitated())
    {
        Exit();
        return;
    }

    // リーダーとの距離が停止距離以内の場合は移動を停止して終了する
    if (distanceSq <= stopDistanceSq_)
    {
        Move::Update();
        return;
    }

    // リーダーへの方向を計算して移動入力を設定する
    const Vector2 moveDirection = Vector2(toLeader.x, toLeader.z).Normalize();
    owner_->SetMoveInputXZ(moveDirection, moveSpeed_);
}