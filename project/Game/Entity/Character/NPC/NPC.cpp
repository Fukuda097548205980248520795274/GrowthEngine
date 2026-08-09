#include "NPC.h"
#include "BattleDirector/BattleDirector.h"
#include "Action/Move/Move.h"
#include "HUD/HP/HP.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeEditor/BehaviorTreeEditor.h"

namespace
{
	// NPCが構え状態になる距離
	constexpr float kNpcStanceEnterDistance = 4.5f;
	constexpr float kNpcStanceEnterDistanceSq = kNpcStanceEnterDistance * kNpcStanceEnterDistance;

	// NPCが構え状態を解除する距離
	constexpr float kNpcStanceExitDistance = 5.5f;
	constexpr float kNpcStanceExitDistanceSq = kNpcStanceExitDistance * kNpcStanceExitDistance;
}

/// @brief コンストラクタ
/// @param initData 
/// @param navMesh 
NPC::NPC() : Character()
{
	// 構え状態でなくてもロックオン候補を更新する
	canLockOnWithoutStance_ = true;
}

/// @brief プールに返却したときの処理
void NPC::PoolRelease()
{
	// 当たり判定の削除
	if (landingCollision_)landingCollision_->Delete();
	landingCollision_ = nullptr;

	if (wallTouchCollision_) wallTouchCollision_->Delete();
	wallTouchCollision_ = nullptr;

	// 死亡処理
	Dead();
}

/// @brief 初期化
void NPC::Initialize(const CharacterInitData& initData, CharacterTag characterTag, const NavMesh* navMesh)
{
	// インスタンスリストに登録する
	characters_.push_back(this);

	// タグを設定する
	characterTag_ = characterTag;

	// 攻撃性の値を設定する
	aggressiveness_ = initData.aggressiveness;

	// 初期化データを設定する
	SetInitData(initData);

	// ステートを初期化する
	stateMachine_->ChangeState("None");

	// ナビゲーションメッシュを設定する
	navMesh_ = navMesh;
}

void NPC::Update()
{
	// カットシーン中は移動を停止して、基底クラスの更新処理のみ行う
	if (Character::IsCutsceneActive() || !updateEnabled_)
	{
		// ステートをNoneに変更する
		stateMachine_->ChangeState("None");
		MoveStop();

		Character::Update();
		return;
	}

	// ゲーム終了時は移動を停止して、基底クラスの更新処理のみ行う
	if (Character::IsGameFinished())
	{
		MoveStop();
		Character::Update();
		return;
	}

	// デルタタイムを取得する
	float dt = engine_->GetDeltaTime() * engine_->GetTimeScale();

	// 動けない状態かどうか
	bool isIncapacitated = IsIncapacitated();

	// 更新処理開始前のリセット
	StartUpdate();

	// 攻撃のクールタイムの更新
	UpdateAttackCooltime(dt);

	// ビヘイビアツリーの更新
	if (currentBehaviorTree_)
	{
		// ツリーを実行し、状態を取得
		BehaviorTree::State state = currentBehaviorTree_->Exec();

		// ツリーの実行が終了（成功 or 失敗）したら、予約されていたツリーに切り替える
		if (state == BehaviorTree::State::Success || state == BehaviorTree::State::Failure)
		{
			if (isChangeBehaviorTree_ && !nextBehaviorTree_)
			{
				// 現在のツリーを中断
				currentBehaviorTree_->Abort();
				currentBehaviorTree_ = nextBehaviorTree_;
			}
			else if (isChangeBehaviorTree_ && currentBehaviorTree_->GetName() != nextBehaviorTree_->GetName())
			{
				// 予約されているツリーが存在し、今のツリーと違う名前のツリーなら切り替える

				// 現在のツリーを中断
				currentBehaviorTree_->Abort();

				currentBehaviorTree_ = nextBehaviorTree_;
				currentBehaviorTree_->InitState();

				nextBehaviorTree_ = nullptr; // 予約をクリア
			}
			else if (isChangeBehaviorTree_)
			{
				// 予約されているツリーが存在するが、今のツリーと同じ名前のツリーなら、予約をクリアする
				nextBehaviorTree_ = nullptr;
			}

			isChangeBehaviorTree_ = false;
		}
	}

	// アクションの更新
	ActionUpdate();

	// 動けない状態なら、攻撃トークンを返却して、基底クラスの更新処理を行って終了する
	if (isIncapacitated)
	{
		BattleDirector battleDirector = BattleDirector::GetInstance();

		// 攻撃トークンを返却する
		battleDirector.ReleaseAttackToken(this);

		Character::Update();
		return;
	}

	// 基底クラスの更新
	Character::Update();

	// ターゲットとの距離で構え状態を更新する
	UpdateStanceStateByTargetDistance();
}

/// @brief ターゲットとの距離で構え状態を更新する
void NPC::UpdateStanceStateByTargetDistance()
{
	// ターゲットがいない場合は構え状態を解除する
	if (!lockOnTarget_ || IsInAttackSequence())
	{
		isStance_ = false;
		return;
	}

	// ターゲットとの水平距離を計算する
	Vector3 toTarget = lockOnTarget_->GetWorldPosition() - GetWorldPosition();
	toTarget.y = 0.0f;

	// ターゲットとの距離の二乗を計算する
	const float kDistanceSq = toTarget.x * toTarget.x + toTarget.z * toTarget.z;

	// ヒステリシスを持たせてチャタリングを防ぐ
	if (isStance_)
	{
		// すでに構え状態の場合は、少し遠ざかるまで構えを解除しない
		if (kDistanceSq > kNpcStanceExitDistanceSq)
		{
			isStance_ = false;
		}
	}
	else
	{
		// 構え状態でない場合は、少し近づくまで構え状態にしない
		if (kDistanceSq <= kNpcStanceEnterDistanceSq)
		{
			isStance_ = true;
			
		}
	}
}

/// @brief 描画処理
void NPC::Draw()
{
	// モデルを描画する
	if(model_)model_->Draw();

	// 攻撃トレイルを描画する
	if (attackTrail_)attackTrail_->Draw();
}

/// @brief 死亡処理
void NPC::Dead()
{
	// ビヘイビアツリーをクリアする
	currentBehaviorTree_ = nullptr;
	nextBehaviorTree_ = nullptr;

	// 基底クラスの死亡処理
	Character::Dead();
}

/// @brief ビヘイビアツリーの変更をリクエストする
/// @param newTree 
void NPC::RequestBehaviorTreeChange(BehaviorTree* newTree)
{
	// 新しいビヘイビアツリーが現在のビヘイビアツリーと同じ名前の場合は、何もしない
	if (newTree && currentBehaviorTree_ && (newTree->GetName() == currentBehaviorTree_->GetName()))
		return;

	// 現在のビヘイビアツリーが存在しない、または現在のビヘイビアツリーが成功または失敗状態の場合は、すぐに新しいビヘイビアツリーに切り替える
	if (!currentBehaviorTree_ ||
		currentBehaviorTree_->GetCurrentState() == BehaviorTree::State::Success ||
		currentBehaviorTree_->GetCurrentState() == BehaviorTree::State::Failure)
	{
		// 現在のビヘイビアツリーが存在する場合は中断する
		if (currentBehaviorTree_)
			currentBehaviorTree_->Abort();

		// 新しいビヘイビアツリーに切り替える
		currentBehaviorTree_ = newTree;
		if (currentBehaviorTree_)currentBehaviorTree_->InitState();
		nextBehaviorTree_ = nullptr;

		// 既に変更が完了しているので、フラグをリセットする
		isChangeBehaviorTree_ = false;
	}
	else
	{
		// 現在のビヘイビアツリーが実行中の場合は、次のビヘイビアツリーとして設定する
		nextBehaviorTree_ = newTree;

		// ビヘイビアツリーの変更がリクエストされたことを示すフラグを立てる
		isChangeBehaviorTree_ = true;
	}
}

/// @brief 攻撃クールタイムの更新処理
/// @param deltaTime 
void NPC::UpdateAttackCooltime(float deltaTime)
{
	// 既にクールタイムが0以下なら何もしない
	if (attackCooltime_ <= 0.0f)return;

	// クールタイムを減らす
	attackCooltime_ -= deltaTime;
	attackCooltime_ = std::max(0.0f, attackCooltime_);
}

/// @brief ビヘイビアツリーを初期化する
/// @param behaviorTreeConfig 
/// @param behaviorTreeEditor 
void NPC::InitBehaviorTree(const BehaviorTreeConfig& behaviorTreeConfig, BehaviorTreeEditor* behaviorTreeEditor)
{
	assert(behaviorTreeEditor);

	// None状態のビヘイビアツリーを設定する
	stateMachine_->GetState("None")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.noneStateBT, this));
	stateMachine_->GetState("Dash")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.dashStateBT, this));
	stateMachine_->GetState("Grabbed")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.grabbedStateBT, this));
	stateMachine_->GetState("Grabbing")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.grabbingStateBT, this));
	stateMachine_->GetState("Guard")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.guardStateBT, this));
	stateMachine_->GetState("LightDamage")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.lightDamageStateBT, this));
	stateMachine_->GetState("HeavyDamage")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.heavyDamageStateBT, this));
	stateMachine_->GetState("DownFalling")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.downFallingStateBT, this));
	stateMachine_->GetState("DownLying")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.downLyingStateBT, this));
	stateMachine_->GetState("DownGettingUp")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.downGettingUpStateBT, this));
	stateMachine_->GetState("DownStagger")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.downStaggerStateBT, this));
	stateMachine_->GetState("BlownAway")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.blownAwayStateBT, this));
	stateMachine_->GetState("BlownFalling")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.blownFallingStateBT, this));
	stateMachine_->GetState("Repel")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.repelStateBT, this));
	stateMachine_->GetState("Deflect")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.deflectStateBT, this));
	stateMachine_->GetState("Repelled")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.repelledStateBT, this));
	stateMachine_->GetState("Deflected")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.deflectedStateBT, this));
	stateMachine_->GetState("Avoid")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.avoidStateBT, this));
	stateMachine_->GetState("Dead")->SetBehaviorTree(behaviorTreeEditor->CreateTree(behaviorTreeConfig.deadStateBT, this));
}

/// @brief ロックオンしているターゲットを検索する
void NPC::SearchLockOnTarget()
{
	// 最も視線方向に近い相手を探す
	float bestDistance = std::numeric_limits<float>::max();
	float bestDot = -1.0f;

	// 最小の被ターゲット数
	int minTargetedCount = std::numeric_limits<int>::max();

	// 攻撃中の相手を見つけたかどうかのフラグ
	bool hasFoundAttackingTarget = false;

	const Vector3 kSelfPosition = GetWorldPosition();

	// ロックオン対象の側を決定する
	const bool kIsSelfPlayerSide = IsPlayerSide();

	for (Character* character : characters_)
	{
		// 無効または自分自身は除外する
		if (!character || character == this)continue;

		// 自分と同じ側の相手は除外する
		if (kIsSelfPlayerSide == character->IsPlayerSide()) continue;

		// 死んでいる相手は除外する
		if (character->IsDead())continue;

		// 自分から相手へのベクトルを計算する
		Vector3 toTarget = character->GetWorldPosition() - kSelfPosition;
		toTarget.y = 0.0f;

		// 距離の二乗を計算する
		const float kDistanceSq = toTarget.x * toTarget.x + toTarget.z * toTarget.z;
		if (kDistanceSq <= 0.0f)
			continue;


		// 自分と同勢力のキャラクターから何人にターゲットされているかをカウント
		int targetedCount = 0;
		for (Character* other : characters_)
		{
			if (other == this || other->IsDead()) continue;

			// 自分と同じ側の勢力で、かつ現在の評価対象(character)をロックオンしているか
			if (other->IsPlayerSide() == kIsSelfPlayerSide && other->GetLockOnTarget() == character)
			{
				targetedCount++;
			}
		}

		// 距離を登録する
		const float kDistance = std::sqrt(kDistanceSq);

		// 優先度1: 被ターゲット数が少ない相手を選ぶ
		if (targetedCount < minTargetedCount)
		{
			minTargetedCount = targetedCount;
			bestDistance = kDistance;
			lockOnTarget_ = character;
		}
		else if (targetedCount == minTargetedCount && kDistance < bestDistance)
		{
			// 優先度2: 被ターゲット数が同じ場合は、距離が近い相手を選ぶ
			bestDistance = kDistance;
			lockOnTarget_ = character;
		}
	}
}

/// @brief デバッグUIを描画する
/// @param placementData 
/// @param placementList 
/// @param history 
/// @param isDirty 
void NPC::DrawDebugUI(PlacementData* placementData, std::vector<PlacementData>& placementList, StageEditorHistory* history, bool* isDirty)
{
	// キャラクターの状態を表示する
	Entity::DrawDebugUI(placementData, placementList, history, isDirty);

	ImGui::Separator();

	// 攻撃性を表示・編集する
	if (ImGui::IsItemActivated()) { history->SaveHistory(placementList); *isDirty = true; }
	ImGui::DragFloat("攻撃性", &aggressiveness_, 1, 0, 1000000);
	if (ImGui::IsItemDeactivatedAfterEdit())placementData->aggressiveness = aggressiveness_;
}