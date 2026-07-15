#include "NPC.h"
#include "BattleDirector/BattleDirector.h"
#include "Action/Move/Move.h"
#include "HUD/HP/HP.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeEditor/BehaviorTreeEditor.h"

namespace
{
	constexpr float kNpcStanceDistance = 5.0f;
	constexpr float kNpcStanceDistanceSq = kNpcStanceDistance * kNpcStanceDistance;
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


	// 初期化データを設定する
	SetInitData(initData);

	// ステートを初期化する
	stateMachine_->ChangeState("None");

	switch (characterTag_)
	{
		// 味方は青色にする
	case CharacterTag::Ally:
		model_->param_->meshMaterial[0].color = Vector4(0.1f, 0.1f, 1.0f, 1.0f);
		break;

		// VIPは緑色にする
	case CharacterTag::Vip:
		model_->param_->meshMaterial[0].color = Vector4(0.1f, 1.0f, 0.1f, 1.0f);
		break;

		// 敵は赤色にする
	case CharacterTag::EnemyNormal:
		model_->param_->meshMaterial[0].color = Vector4(1.0f, 0.1f, 0.1f, 1.0f);
		break;

		// ボスは紫色にする
	case CharacterTag::EnemyBoss:
		model_->param_->meshMaterial[0].color = Vector4(1.0f, 0.1f, 1.0f, 1.0f);
		break;
	}


	// ナビゲーションメッシュを設定する
	navMesh_ = navMesh;
}

void NPC::Update()
{
	// 更新が無効なら何もしない
	if (!updateEnabled_)return;

	// カットシーン中は移動を停止して、基底クラスの更新処理のみ行う
	if (Character::IsCutsceneActive())
	{
		// ステートをNoneに変更する
		stateMachine_->ChangeState("None");
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
			// 予約されているツリーが存在し、今のツリーと違う名前のツリーなら切り替える
			if (nextBehaviorTree_ && currentBehaviorTree_->GetName() != nextBehaviorTree_->GetName())
			{
				// 現在のツリーを中断
				currentBehaviorTree_->Abort();

				currentBehaviorTree_ = nextBehaviorTree_;
				nextBehaviorTree_ = nullptr; // 予約をクリア
			}
			else if (nextBehaviorTree_)
			{
				// 予約されているツリーが存在するが、今のツリーと同じ名前のツリーなら、予約をクリアする
				nextBehaviorTree_ = nullptr;
			}
		}
	}

	// アクションの更新
	ActionUpdate();

	// 動けない状態なら、攻撃トークンを返却して、基底クラスの更新処理を行って終了する
	if (isIncapacitated)
	{
		// 攻撃トークンを返却する
		BattleDirector::GetInstance().ReleaseAttackToken(this);

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
	if (!lockOnTarget_ || IsInAttackSequence())
	{
		isStance_ = false;
		return;
	}

	Vector3 toTarget = lockOnTarget_->GetWorldPosition() - GetWorldPosition();
	toTarget.y = 0.0f;

	const float distanceSq = toTarget.x * toTarget.x + toTarget.z * toTarget.z;
	isStance_ = (distanceSq <= kNpcStanceDistanceSq);
}

/// @brief スタイルが変化したときの処理
/// @param newStyle 
void NPC::OnStyleChanged(FightStyle newStyle)
{
	// スタイルに応じたビヘイビアツリーを生成して設定する
	//behaviorTree_ = CreateBehaviorTreeForStyle(newStyle);
}

/// @brief スタイルに応じたビヘイビアツリーを生成する
/// @param style 
/// @return 
std::unique_ptr<BehaviorTree> NPC::CreateBehaviorTreeForStyle(FightStyle style)
{
	// ビヘイビアツリー
	std::unique_ptr<BehaviorTree> behaviorTree = nullptr;

	switch (style)
	{
	// 旋嵐スタイル
	case FightStyle::Tempest:

		break;

	// 撃鉄スタイル
	case FightStyle::Hammer:

		break;
	}

	return behaviorTree;
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
	// 現在のビヘイビアツリーが存在しない、または現在のビヘイビアツリーが成功または失敗状態の場合は、すぐに新しいビヘイビアツリーに切り替える
	if (!currentBehaviorTree_ ||
		currentBehaviorTree_->GetCurrentState() == BehaviorTree::State::Success ||
		currentBehaviorTree_->GetCurrentState() == BehaviorTree::State::Failure)
	{
		// 現在のビヘイビアツリーが存在する場合は中断する
		if (currentBehaviorTree_)
			currentBehaviorTree_->Abort();

		currentBehaviorTree_ = newTree;
		nextBehaviorTree_ = nullptr;
	}
	else
	{
		// 現在のビヘイビアツリーが実行中の場合は、次のビヘイビアツリーとして設定する
		nextBehaviorTree_ = newTree;
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