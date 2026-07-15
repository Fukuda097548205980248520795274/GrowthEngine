#include "NPC.h"
#include "BattleDirector/BattleDirector.h"
#include "Action/Move/Move.h"
#include "HUD/HP/HP.h"

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
void NPC::Initialize(const CharacterInitData& initData, CharacterTag characterTag, std::unique_ptr<BehaviorTree> behaviorTree, const NavMesh* navMesh)
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

	// ビヘイビアツリーを設定する
	behaviorTree_ = std::move(behaviorTree);
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

	// 動ける状態なら、ビヘイビアツリーを実行する
	if (!isIncapacitated)
	{
		if (currentBehaviorTree_)
		{
			// ツリーを実行し、状態を取得
			BehaviorTree::State state = currentBehaviorTree_->Exec();

			// ツリーの実行が終了（成功 or 失敗）したら、予約されていたツリーに切り替える
			if (state == BehaviorTree::State::Success || state == BehaviorTree::State::Failure)
			{
				if (nextBehaviorTree_)
				{
					currentBehaviorTree_ = nextBehaviorTree_;
					nextBehaviorTree_ = nullptr; // 予約をクリア
				}
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
	behaviorTree_ = CreateBehaviorTreeForStyle(newStyle);
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